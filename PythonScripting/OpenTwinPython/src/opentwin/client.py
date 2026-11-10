# @otlicense

from opentwin.config import OpenTwinConfig
from opentwin._internal.Authentication import Authentication
from opentwin._internal.Communication import Communication

class OpenTwinClient:
    def __init__(self, global_session_service_url: str, login_token: str) -> None:
        self._print("Initializing OpenTwinClient...")

        # Ensure configuration is valid
        OpenTwinConfig().invariant()

        # Initialize communication
        self._communication = Communication(global_session_service_url)

        # Initialize authentication
        self._print(f"Connection established with Global Session Service at {global_session_service_url}.")
        self._print(f"Attempting to authenticate with login token: {login_token}")
        self._auth = Authentication(self._communication, login_token)

        self._print("OpenTwinClient initialized successfully.")

    ################################################################################################################################

    @staticmethod
    def _print(message: str) -> None:
        print("[OpenTwin] [CLIENT] " + message)