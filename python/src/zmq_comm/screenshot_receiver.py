import zmq
import logging

class ScreenshotReceiver:
    """
    ScreenshotReceiver listens to a ZeroMQ endpoint (using a PULL socket)
    and receives a BMP image buffer sent from the C++ application.

    Attributes:
        endpoint (str): The ZeroMQ endpoint to connect to (e.g., "tcp://localhost:5555").
    """

    def __init__(self, endpoint: str):
        """
        Initialize the receiver with the given endpoint and set up the ZeroMQ context.

        :param endpoint: ZeroMQ endpoint URL.
        """
        self.endpoint = endpoint
        self.context = zmq.Context()
        self.socket = self.context.socket(zmq.PULL)
        try:
            self.socket.bind(self.endpoint)
            logging.info(f"ScreenshotReceiver bound to endpoint: {self.endpoint}")
        except Exception as e:
            logging.error(f"Error binding to endpoint {self.endpoint}: {e}")
            raise

    def receive_image(self) -> bytes:
        """
        Receives a BMP image buffer from the ZeroMQ socket.

        :return: The raw image data as bytes.
        """
        try:
            message = self.socket.recv()
            logging.info(f"Received image buffer of size {len(message)} bytes.")
            return message
        except Exception as e:
            logging.error(f"Error receiving image: {e}")
            return b''

    def close(self):
        """
        Closes the ZeroMQ socket and terminates the context.
        """
        self.socket.close()
        self.context.term()
        logging.info("ScreenshotReceiver closed.")
