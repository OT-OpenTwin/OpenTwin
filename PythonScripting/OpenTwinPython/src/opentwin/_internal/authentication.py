from typing import Any

from .communication import Communication

class Authentication:

    """
    A class for handling authentication with the OpenTwin services.

    This class provides methods for logging in and managing authentication tokens.

    Attributes:
        _io (Communication): The communication instance used to interact with the OpenTwin services.
    """
    def __init__(self, communication: Communication):
        self._io = communication

    def login(self, authUrl: str, token: str) -> dict[str, Any]:

        request_data = {
            "message": "Hello from OpenTwin Python Module",
            "token": token
        }

        x = self._io.sendTo2(authUrl, request_data)
        return x
