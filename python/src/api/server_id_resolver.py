from typing import Any

import requests
import logging

class ServerIdResolver:
    """
    ServerIdResolver is responsible for retrieving the long (13-digit) game ID
    for a Battlefield V server using an external API.
    """

    BASE_URL = "https://api.gametools.network/bfv/servers/"

    def get_game_id(self, server_short_id: str) -> Any | None:
        """
        Retrieves the 13-digit game ID from the API using the short server ID.

        :param server_short_id: The short server ID (4-5 digits) extracted via OCR.
        :return: The corresponding game ID as a string, or None if not found.
        """
        params = {
            "name": server_short_id,
            "region": "all",
            "limit": 12,
            "platform": "pc"
        }
        try:
            response = requests.get(self.BASE_URL, params=params, timeout=5)
            response.raise_for_status()
            data = response.json()
            servers = data.get("servers", [])
            matching_server = next((s for s in servers if f"#{server_short_id}" in s.get("prefix", "")), None)
            if matching_server:
                game_id = matching_server.get("gameId")
                logging.info(f"Resolved game ID: {game_id} for short ID: {server_short_id}")
                return game_id
            else:
                logging.warning(f"Server with short ID {server_short_id} not found.")
                return None
        except requests.RequestException as e:
            logging.error(f"Failed to fetch game ID for short ID {server_short_id}: {e}")
            return None
