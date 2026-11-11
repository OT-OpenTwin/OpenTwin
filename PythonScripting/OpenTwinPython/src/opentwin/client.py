# @otlicense

# OpenTwin
from opentwin.config import Config

# Logging
import logging
logger = logging.getLogger("OpenTwin")

# OpenTwin internal
class OpenTwinClient:
    def __init__(self, global_session_service_url: str, login_token: str, config: Config = None) -> None:
        
        from opentwin._internal.system.FrontendLauncher import FrontendLauncher
        from opentwin._internal.Authentication import Authentication
        from opentwin._internal.Communication import Communication

        logger.info("Initializing OpenTwinClient...")

        if config is None:
            logger.info("No configuration provided. Using default configuration.")
            config = Config()


        logger.debug(f"Initializing FrontendLauncher with executable path \"{config.frontend_app_path}\"...")
        uiLauncher = FrontendLauncher(config.frontend_app_path)

        # Initialize communication
        logger.debug(f"Initializing Communication with Global Session Service at \"{global_session_service_url}\"...")
        self._communication = Communication(global_session_service_url, config)
        
        # Initialize authentication
        logger.debug(f"Attempting to authenticate with login token \"{login_token}\"...")
        self._auth = Authentication(self._communication, login_token)

        logger.info("OpenTwinClient initialized successfully.")
