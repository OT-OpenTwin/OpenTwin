# @otlicense

# Python standard
import json
import logging
logger = logging.getLogger("OpenTwin")

# OpenTwin
from opentwin_api._internal.io.gateway_communication import GatewayCommunication
from opentwin_api.project import Project

# OpenTwin internal
from opentwin_api._internal.authentication import Authentication
from opentwin_api._internal.io.communication import Communication
from opentwin_api._internal.default_config import DefaultConfig
from opentwin_api._internal.launching.ui_launch_data import UiLaunchData
from opentwin_api._internal.system.frontend_launcher import FrontendLauncher

# OpenTwin internal
class OpenTwinAPI:

    @classmethod
    def open_project(cls, 
                 token: str,
                 url: str,
                 name: str,
                 version: str | None = None,
                 certificate_path: str | None = None,
                 frontend_path: str | None = None
                 ) -> Project:
        
        logger.info("Initializing OpenTwinClient...")

        # Ceck mandatory parameters

        if not token:
            raise ValueError("token must be provided")

        if not url:
            raise ValueError("url must be provided")

        if not name:
            raise ValueError("name must be provided")

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
            project_name=name,
            project_version=version,
            gss_url=url
        )

        # Launch the frontend application
        logger.info("Launching OpenTwin frontend application...")
        uiLauncher.launch(uiLaunchData)

        #gateway_communication = GatewayCommunication.try_connect(
        #    project_name=project_name,
        #    gateway_ticket=LaunchTickets.gateway_launch_ticket,
        #    communication=communication
        #)

        project = Project(name)

        return project
