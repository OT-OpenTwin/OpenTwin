# @otlicense

# Python standard
import json

# OpenTwin
from opentwin._internal.DefaultConfig import DefaultConfig

# OpenTwin internal
from opentwin._internal.Authentication import Authentication
from opentwin._internal.Communication import Communication
from opentwin._internal.launching.ui_launch_data import UiLaunchData
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
            certificate_path = DefaultConfig().ca_certificate_path

        if frontend_path is None:
            frontend_path = DefaultConfig().frontend_app_path

        # Initialize the FrontendLauncher
        uiLauncher = FrontendLauncher(frontend_path)

        # Initialize communication
        communication = Communication(url, certificate_path)
        
        # Initialize authentication
        logger.debug(f"Attempting to authenticate with login token \"{token}\"...")

        LaunchTickets = Authentication.generate_launch_tickets(communication, token)

        # Prepare login session data to pass to the frontend
        uiLaunchData = UiLaunchData(
            ticket=LaunchTickets.ui_launch_ticket,
            project=project,
            gss_url=url
        )

        # Launch the frontend application
        logger.info("Launching OpenTwin frontend application...")
        uiLauncher.launch(uiLaunchData)

        logger.info("OpenTwinClient initialized successfully.")
