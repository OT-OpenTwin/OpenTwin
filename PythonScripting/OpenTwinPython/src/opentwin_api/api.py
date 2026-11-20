# @otlicense

# Python standard
import time
import logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger("OpenTwin")

# OpenTwin
from opentwin_api.project import Project

# OpenTwin internal
from opentwin_api._internal.io.communication import Communication
from opentwin_api._internal.io.gateway_communication import GatewayCommunication
from opentwin_api._internal.default_config import DefaultConfig
from opentwin_api._internal.launching.ui_launch_data import UiLaunchData
from opentwin_api._internal.launching.launch_tickets import LaunchTickets
from opentwin_api._internal.system.frontend_launcher import FrontendLauncher
from opentwin_api._internal.service.lss_api import LSSAPI

# OpenTwin internal
class OpenTwinAPI:

    @classmethod
    def open_project(cls, 
                 token: str,
                 url: str,
                 name: str,
                 version: str | None = None,
                 certificate_path: str | None = None,
                 frontend_path: str | None = None,
                 wait_for_project_timeout_sec: int = 300,
                 gateway_connection_timeout_sec: int = 60,
                 initial_wait_for_project_timeout_sec: int = 5
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
        auth = communication.getAuth()
        gss = communication.getGSS()

        # Login and obtain a user session token
        logger.info(f"Logging in with token \"{token}\"...")
        user_session_token = auth.login(token)

        # Determine project collection
        project_collection = auth.get_project_collection_from_name(user_session_token, name)
        project_id = name + ":" + project_collection

        # Generate launch tickets
        logger.debug(f"Generating launch tickets ...")
        launchTickets: LaunchTickets = auth.generate_launch_tickets(user_session_token)

        # Prepare login session data to pass to the frontend
        uiLaunchData = UiLaunchData(
            ticket=launchTickets.ui_launch_ticket,
            project_name=name,
            project_version=version,
            gss_url=url
        )

        # Launch the frontend application
        logger.info("Launching OpenTwin frontend application...")
        uiLauncher.launch(uiLaunchData)

        # Initially sleep since the frontend takes time to open the project
        if initial_wait_for_project_timeout_sec <= 0:
            initial_wait_for_project_timeout_sec = 5
        time.sleep(initial_wait_for_project_timeout_sec)

        # Initialize the timeout
        if wait_for_project_timeout_sec <= 0:
            wait_for_project_timeout_sec = 3600 # Default to 1 hour if not specified
        start_time = time.monotonic()
        deadline_time = start_time + wait_for_project_timeout_sec

        # Determine the Local Session Service (LSS) URL for the project
        lss_url : str | None = None

        while time.monotonic() < deadline_time:
            lss_url = gss.determine_lss_for_project(project_id)
            if lss_url is not None:
                break
            else:
                time.sleep(2)

        if lss_url is None:
            raise RuntimeError(f"Project \"{name}\" did not become available in the GSS within the timeout period of {wait_for_project_timeout_sec} seconds")

        logger.info(f"Determined Local Session Service (LSS: {lss_url}). Waiting for project to become available in the LSS...")
        lss = LSSAPI(lss_url, communication)

        # Wait for project open completed
        start_time = time.monotonic()
        deadline_time = start_time + wait_for_project_timeout_sec
        project_opened = False

        while time.monotonic() < deadline_time:
            project_opened = lss.check_project_opened(project_id=project_id)
            
            if project_opened is True:
                break
            else:
                time.sleep(2)

        if project_opened is False:
            raise RuntimeError(f"Project \"{name}\" did not become available in the LSS within the timeout period of {wait_for_project_timeout_sec} seconds")

        # Project is now open and all services running (including ui)
        logger.info(f"Project \"{name}\" is now open and available in the LSS. Requesting gateway connection...")

        gateway_connection_info = lss.request_gateway_launch(
            project_id=project_id
        )

        if gateway_connection_info is None:
            raise RuntimeError(f"Failed to obtain gateway connection information for project \"{name}\"")

        # Establish connection with gateway
        #gateway_communication = GatewayCommunication.try_connect(
        #    project_name=project_name,
        #    gateway_ticket=LaunchTickets.gateway_launch_ticket,
        #    communication=communication
        #)

        project = Project(
            name=name,
            id=project_id
        )

        return project
