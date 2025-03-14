import logging
import zmq
from zmq_comm.screenshot_receiver import ScreenshotReceiver
from ocr.ocr_processor import OCRProcessor
from api.server_id_resolver import ServerIdResolver
from api.cheater_count_fetcher import CheaterCountFetcher

def main():
    logging.basicConfig(level=logging.INFO)
    context = zmq.Context()

    # Socket pour envoyer la réponse vers C++
    sender = context.socket(zmq.PUSH)
    sender.bind("tcp://*:5556")

    # Socket pour recevoir les captures d’écran
    receiver = ScreenshotReceiver("tcp://*:5555")

    # Socket pour recevoir la commande d'arrêt
    shutdown_socket = context.socket(zmq.REP)
    shutdown_socket.bind("tcp://*:5557")

    # Initialisation des modules
    ocr_processor = OCRProcessor()
    resolver = ServerIdResolver()
    fetcher = CheaterCountFetcher()

    logging.info("Backend is listening...")

    # Création d'un poller pour écouter à la fois le receiver et le shutdown
    poller = zmq.Poller()
    poller.register(receiver.socket, zmq.POLLIN)
    poller.register(shutdown_socket, zmq.POLLIN)

    try:
        while True:
            socks = dict(poller.poll(1000))  # timeout 1 seconde
            if shutdown_socket in socks:
                msg = shutdown_socket.recv_string()
                logging.info("Shutdown command received: {}".format(msg))
                shutdown_socket.send_string("OK")
                break

            if receiver.socket in socks:
                bmp_buffer = receiver.receive_image()
                if not bmp_buffer:
                    logging.warning("Received an empty image. Skipping...")
                    sender.send_json({"error": "Received an empty image."})
                    continue

                # Traitement de l'image
                server_short_id = ocr_processor.process_image(bmp_buffer)
                if not server_short_id or server_short_id == "No number found":
                    logging.error("Failed to extract server short ID from OCR.")
                    sender.send_json({"error": "Failed to extract server short ID from OCR."})
                    continue

                logging.info("Extracted server short ID: {}".format(server_short_id))

                game_id = resolver.get_game_id(server_short_id)
                if not game_id:
                    logging.error("Failed to retrieve game ID from Gametools API.")
                    sender.send_json({"error": "to retrieve game ID from Gametools API."})
                    continue

                cheater_count = fetcher.get_cheater_count(game_id)
                if cheater_count is None:
                    logging.error("Failed to retrieve cheater count from BFVHackers API.")
                    sender.send_json({"error": "to retrieve cheater count from BFVHackers API."})
                    continue

                logging.info("Server {} ({}): {} cheaters detected.".format(server_short_id, game_id, cheater_count))
                sender.send_json({"cheater_count": cheater_count})
                logging.info("Sent cheater count to overlay.")

    except KeyboardInterrupt:
        logging.info("KeyboardInterrupt received. Shutting down backend...")
    finally:
        receiver.close()
        sender.close()
        shutdown_socket.close()
        context.term()

if __name__ == "__main__":
    main()
