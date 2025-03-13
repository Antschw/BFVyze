from typing import Any

import requests
import logging

class CheaterCountFetcher:
    """
    CheaterCountFetcher retrieves the number of cheaters present on a server
    by querying the external BFVHackers API.
    """

    BASE_URL = "https://bfvhackers.com/api/v1/server-hackers"

    def get_cheater_count(self, game_id: str) -> Any | None:
        """
        Retrieves the number of cheaters for the server with the given game ID.

        :param game_id: The 13-digit game ID of the server.
        :return: The number of cheaters as an integer, or None if an error occurs.
        """
        params = {"server-id": game_id}
        try:
            response = requests.get(self.BASE_URL, params=params, timeout=5)
            response.raise_for_status()
            data = response.json()
            num_cheaters = data.get("num_hackers", 0)
            logging.info(f"Cheater count for game ID {game_id}: {num_cheaters}")
            return num_cheaters
        except requests.RequestException as e:
            logging.error(f"Failed to fetch cheater count for game ID {game_id}: {e}")
            return None
