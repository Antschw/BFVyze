import logging
import re

import cv2
import numpy as np
import pytesseract
from numpy import ndarray


def decode_image(image_bytes: bytes) -> ndarray | None:
    """
    Decodes a BMP image from the given bytes buffer into a grayscale OpenCV image.

    :param image_bytes: The BMP image data as bytes.
    :return: The decoded image as a numpy array, or None if decoding fails.
    """
    try:
        image_array = np.frombuffer(image_bytes, dtype=np.uint8)
        image = cv2.imdecode(image_array, cv2.IMREAD_GRAYSCALE)
        if image is None:
            logging.error("Failed to decode image from bytes.")
        else:
            logging.info(f"Image decoded successfully with shape {image.shape}.")
        return image
    except Exception as e:
        logging.error(f"Exception during image decoding: {e}")
        return None


def preprocess_image(image: np.ndarray) -> np.ndarray:
    """
    Preprocess the decoded image to optimize OCR performance.

    Steps:
     - Apply a median blur to reduce noise.
     - Invert the image colors (to have a dark text on a light background).
     - Apply OTSU thresholding to binarize the image.

    :param image: Input grayscale image.
    :return: The preprocessed (binary) image.
    """
    try:
        processed = cv2.medianBlur(image, 3)
        processed = cv2.bitwise_not(processed)
        _, processed = cv2.threshold(processed, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)
        logging.info("Image preprocessed successfully for OCR.")
        return processed
    except Exception as e:
        logging.error(f"Error during image preprocessing: {e}")
        return image


def extract_server_id(image: np.ndarray) -> str:
    """
    Applies OCR to the preprocessed image to extract the server ID.
    The expected format is a 4 to 5-digit number, optionally preceded by '#' or space.

    :param image: Preprocessed image (binary) ready for OCR.
    :return: The extracted server ID as a string, or an error message if not found.
    """
    try:
        # OCR configuration optimized for numeric extraction (psm 6: uniform block of text)
        custom_config = r'--oem 1 --psm 6 -c tessedit_char_whitelist=0123456789#'
        raw_text = pytesseract.image_to_string(image, config=custom_config)
        logging.debug(f"OCR raw output: {raw_text}")
        # Extraction: optional '#' or whitespace followed by 4 or 5 digits
        match = re.search(r'#?\s?(\d{4,5})', raw_text)
        if match:
            server_id = match.group(1)
            logging.info(f"Extracted server ID: {server_id}")
            return server_id
        else:
            logging.warning("Server ID not found in OCR output.")
            return "No number found"
    except Exception as e:
        logging.error(f"Exception during OCR extraction: {e}")
        return "No number found"


def process_image(image_bytes: bytes) -> str:
    """
    Full processing pipeline: decode the BMP image, preprocess it, and extract the server ID.

    :param image_bytes: The BMP image data as bytes.
    :return: The extracted server ID, or an empty string if extraction fails.
    """
    image = decode_image(image_bytes)
    if image is None:
        logging.error("Image decoding failed; cannot proceed with OCR.")
        return ""
    processed_image = preprocess_image(image)
    return extract_server_id(processed_image)


class OCRProcessor:
    """
    OCRProcessor handles the image processing and OCR extraction of the server ID
    from a BMP image received via ZeroMQ.

    This class is responsible for:
      - Decoding the BMP image from a bytes buffer into an OpenCV image.
      - Preprocessing the image (denoising, thresholding, inversion) to optimize OCR.
      - Extracting the server ID (a 4 to 5-digit number, optionally preceded by '#')
        from the processed image.
    """

    def __init__(self, tesseract_cmd: str = None):
        """
        Initialize the OCRProcessor and optionally set the path to the Tesseract executable.

        :param tesseract_cmd: Path to tesseract executable.
         If provided, it is set for pytesseract.
        """
        if tesseract_cmd:
            pytesseract.pytesseract.tesseract_cmd = tesseract_cmd
            logging.info(f"Tesseract command set to: {tesseract_cmd}")
