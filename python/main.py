import logging
import zmq
from zmq_comm.screenshot_receiver import ScreenshotReceiver
from ocr.ocr_processor import OCRProcessor
from api.server_id_resolver import ServerIdResolver
from api.cheater_count_fetcher import CheaterCountFetcher

def main():
    logging.basicConfig(level=logging.INFO)

    # Socket ZeroMQ pour envoyer le nombre de cheaters à C++
    context = zmq.Context()
    sender = context.socket(zmq.PUSH)
    sender.bind("tcp://*:5556")  # Port différent de celui de réception

    # Bind le socket PULL pour écouter les captures d’écran sur le port 5555
    receiver = ScreenshotReceiver("tcp://*:5555")

    # Initialisation des modules
    ocr_processor = OCRProcessor()
    resolver = ServerIdResolver()
    fetcher = CheaterCountFetcher()

    logging.info("Backend is listening for screenshots... (Press Ctrl+C to exit)")

    try:
        while True:
            bmp_buffer = receiver.receive_image()
            if not bmp_buffer:
                logging.warning("Received an empty image. Skipping...")
                continue

            # Extraction de l'ID serveur via OCR
            server_short_id = ocr_processor.process_image(bmp_buffer)
            if not server_short_id or server_short_id == "No number found":
                logging.error("Failed to extract server short ID from OCR.")
                continue

            logging.info(f"Extracted server short ID: {server_short_id}")

            # Récupération de l'ID complet via GameTools API
            game_id = resolver.get_game_id(server_short_id)
            if not game_id:
                logging.error("Failed to retrieve game ID from API.")
                continue

            # Récupération du nombre de cheaters via BFVHackers API
            cheater_count = fetcher.get_cheater_count(game_id)
            if cheater_count is None:
                logging.error("Failed to retrieve cheater count from API.")
                continue

            logging.info(f"Server {server_short_id} ({game_id}): {cheater_count} cheaters detected.")

            # Envoi du nombre de cheaters via ZeroMQ
            sender.send_json({"cheater_count": cheater_count})
            logging.info("Sent cheater count to overlay.")

    except KeyboardInterrupt:
        logging.info("KeyboardInterrupt received. Shutting down backend...")

    finally:
        receiver.close()
        sender.close()
        context.term()

if __name__ == "__main__":
    main()
