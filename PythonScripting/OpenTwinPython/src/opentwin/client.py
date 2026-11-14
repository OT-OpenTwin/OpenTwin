# @otlicense

# Python standard
import json

# OpenTwin
from opentwin._internal.DefaultConfig import DefaultConfig

# OpenTwin internal
from opentwin._internal.Authentication import Authentication
from opentwin._internal.Communication import Communication
from opentwin._internal.generated.login_data import LoginData
from opentwin._internal.system.FrontendLauncher import FrontendLauncher

# Logging
import logging
logger = logging.getLogger("OpenTwin")

# OpenTwin internal
class OpenTwinClient:

    """
    The OpenTwinClient is the main entry point for interacting with OpenTwin.

    Arguments:
        token (str): The login token for authentication.
        url (str): The URL of the Global Session Service.
        project (str): The name of the project to connect to.
        certificate_path (str, optional): The full path to the CA certificate file for TLS verification. If not provided, the default configuration will be used.
        frontend_path (str, optional): The full path to the frontend application executable. If not provided, the default configuration will be used.
    """
    def __init__(self, 
                 token: str,
                 url: str,
                 project: str,
                 certificate_path: str = None,
                 frontend_path: str = None
                 ) -> None:
        
        logger.info("Initializing OpenTwinClient...")

        # Ceck mandatory parameters

        if not token:
            raise ValueError("token must be provided")

        if not url:
            raise ValueError("url must be provided")

        if not project:
            raise ValueError("project must be provided")

        # Initialize default parameters

        if certificate_path is None:
            certificate_path = DefaultConfig().certificate_path

        if frontend_path is None:
            frontend_path = DefaultConfig().frontend_path

        # Initialize the FrontendLauncher
        uiLauncher = FrontendLauncher(frontend_path)

        # Initialize communication
        self._communication = Communication(url, certificate_path)
        
        # Initialize authentication
        logger.debug(f"Attempting to authenticate with login token \"{token}\"...")
        self._auth = Authentication(self._communication, token)

        # Prepare login session data to pass to the frontend
        login_data = LoginData(
            original_username=self._auth.original_username,
            db_user=self._auth.session_user,
            db_password=self._auth.session_password
        )

        # Launch the frontend application
        logger.info("Launching OpenTwin frontend application...")
        uiLauncher.launch([
            "--login-session", login_data.to_json()
        ])



        logger.info("OpenTwinClient initialized successfully.")
