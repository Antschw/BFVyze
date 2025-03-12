import logging
from zmq_comm.screenshot_receiver import ScreenshotReceiver
from ocr.ocr_processor import OCRProcessor
from api.server_id_resolver import ServerIdResolver
from api.cheater_count_fetcher import CheaterCountFetcher

def main():
    logging.basicConfig(level=logging.INFO)

    # Bind the PULL socket to listen on port 5555 for incoming BMP buffers
    zmq_endpoint = "tcp://*:5555"
    receiver = ScreenshotReceiver(zmq_endpoint)

    # Initialize OCR processor and API resolvers
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

            # Process the image via OCR to extract the short server ID
            server_short_id = ocr_processor.process_image(bmp_buffer)
            if not server_short_id or server_short_id == "No number found":
                logging.error("Failed to extract server short ID from OCR.")
                continue

            logging.info(f"Extracted server short ID: {server_short_id}")

            # Retrieve the long (13-digit) game ID using the ServerIdResolver
            game_id = resolver.get_game_id(server_short_id)
            if not game_id:
                logging.error("Failed to retrieve game ID from API.")
                continue

            # Retrieve the number of cheaters using CheaterCountFetcher
            cheater_count = fetcher.get_cheater_count(game_id)
            if cheater_count is None:
                logging.error("Failed to retrieve cheater count from API.")
                continue

            logging.info(f"Server {server_short_id} ({game_id}): {cheater_count} cheaters detected.")
    except KeyboardInterrupt:
        logging.info("KeyboardInterrupt received. Shutting down backend...")
    finally:
        receiver.close()

if __name__ == "__main__":
    main()
