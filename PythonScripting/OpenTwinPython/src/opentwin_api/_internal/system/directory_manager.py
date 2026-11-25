# @otlicense

# Python standard
import os
import sys
from pathlib import Path

# External dependencies
from platformdirs import user_data_dir as platformdirs_user_data_dir
from platformdirs import user_config_dir as platformdirs_user_config_dir
from platformdirs import user_cache_dir as platformdirs_user_cache_dir
 
class DirectoryManager:
    @staticmethod
    def get_local_user_data_directory() -> Path:
        return Path(platformdirs_user_data_dir("OpenTwin", appauthor=False))

    @staticmethod
    def get_existing_local_frontend_installation_directory() -> Path | None:
        path = Path(platformdirs_user_data_dir("OpenTwin_Frontend", appauthor=False))
        if path.is_dir():
            return path
        else:
            return None

    @staticmethod
    def get_application_file_extension() -> str:
        if sys.platform == "win32":
            return ".exe"
        else:
            return ""

    @staticmethod
    def get_default_frontend_application_names() -> list[str]:
        return [
            "OpenTwin" + DirectoryManager.get_application_file_extension(),
            "uiFrontend" + DirectoryManager.get_application_file_extension()
        ]
        

    @staticmethod
    def get_existing_local_frontend_application_file() -> Path | None:
        application_path = DirectoryManager.get_existing_local_frontend_installation_directory()

        if application_path is None:
            return None

        application_names = DirectoryManager.get_default_frontend_application_names()
        application_paths = [application_path / name for name in application_names]

        for application_path in application_paths:
            if application_path.is_file():
                return application_path

        return None

    @staticmethod
    def get_existing_local_frontend_ca_file() -> Path | None:
        application_path = DirectoryManager.get_existing_local_frontend_installation_directory()

        if application_path is None:
            return None

        ca_file_path = application_path / "Certificates" / "ca.pem"

        if ca_file_path.is_file():
            return ca_file_path

        return None

    @staticmethod
    def get_local_user_config_directory() -> Path:
        return Path(platformdirs_user_config_dir("OpenTwin", appauthor=False))

    @staticmethod
    def get_local_user_cache_directory() -> Path:
        return Path(platformdirs_user_cache_dir("OpenTwin", appauthor=False))

    @staticmethod
    def get_existing_opentwin_dev_root_directory() -> Path | None:
        devroot = os.environ.get("OPENTWIN_DEV_ROOT")

        if devroot is None:
            return None

        devroot_path = Path(devroot)

        if not devroot_path.is_dir():
            raise FileNotFoundError(f"OPENTWIN_DEV_ROOT is set to \"{devroot}\", but this directory does not exist.")
        
        return devroot_path

    @staticmethod
    def get_existing_opentwin_dev_frontend_file() -> Path | None:
        devroot = DirectoryManager.get_existing_opentwin_dev_root_directory()

        if devroot is None:
            return None

        application_names = DirectoryManager.get_default_frontend_application_names()

        frontend_paths = [devroot / "Deployment" / name for name in application_names]

        for frontend_path in frontend_paths:
            if frontend_path.exists() and frontend_path.is_file():
                return frontend_path

        raise FileNotFoundError(f"OpenTwin frontend application not found in development environment. Please ensure that the frontend is built and the path is correctly set. Searched paths: {frontend_paths}")

    @staticmethod
    def get_existing_opentwin_dev_ca_file() -> Path | None:
        devroot = DirectoryManager.get_existing_opentwin_dev_root_directory()

        if devroot is None:
            return None

        ca_file_path = devroot / "Deployment" / "Certificates" / "ca.pem"

        if ca_file_path.is_file():
            return ca_file_path

        raise FileNotFoundError(f"OpenTwin CA certificate not found in development environment. Please ensure that the frontend is built and the path is correctly set. Searched path: {ca_file_path}")