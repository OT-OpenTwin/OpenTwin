# @otlicense

# Python standard
import os
from pathlib import Path
from typing import Optional

# OpenTwin internal
from opentwin._internal.system.DirectoryManager import DirectoryManager

class Config:

    def __init__(self) -> None:
        self._ca_certificate_path: Path = None
        self._frontend_app_path: Path = None

        self.determine_ca_certificate_path()
        self.determine_frontend_app_path()

    @property
    def ca_certificate_path(self) -> Optional[Path]:
        return self._ca_certificate_path

    @property
    def frontend_app_path(self) -> Optional[Path]:
        return self._frontend_app_path

    ################################################################################################################################

    # Overridable methods

    def determine_ca_certificate_path(self) -> Optional[Path]:
        cert_dir = os.environ.get("OPEN_TWIN_CERTS_PATH")

        # 1: Check for dedicated environment
        if cert_dir:
            self._ca_certificate_path = Path(cert_dir) / "ca.pem"

        if self._ca_certificate_path is None or not self._ca_certificate_path.is_file():
            # 2: Check for development environment
            self._ca_certificate_path = DirectoryManager.get_existing_opentwin_dev_ca_file()

            if self._ca_certificate_path is None or not self._ca_certificate_path.is_file():
                # 3: Check for local installation
                self._ca_certificate_path = DirectoryManager.get_existing_local_frontend_ca_file()

        if self._ca_certificate_path is not None and not self._ca_certificate_path.is_file():
            raise FileNotFoundError(f"CA certificate file not found at: {self._ca_certificate_path}")
        
        return self._ca_certificate_path

    def determine_frontend_app_path(self) -> Optional[Path]:
        self._frontend_app_path = None

        # 1: Check for dedicated environment
        if os.environ.get("OPEN_TWIN_FRONTEND_PATH"):
            self._frontend_app_path = Path(os.environ.get("OPEN_TWIN_FRONTEND_PATH"))
        else:
            # 2: Check for development environment
            self._frontend_app_path = DirectoryManager.get_existing_opentwin_dev_frontend_file()

            # 3: Check for local installation
            if self._frontend_app_path is None:
                self._frontend_app_path = DirectoryManager.get_existing_local_frontend_application_file()

        if self._frontend_app_path is None or not self._frontend_app_path.is_file():
            raise FileNotFoundError("OpenTwin frontend application not found. Please ensure that the frontend is installed and the path is correctly set.")
        
        return self._frontend_app_path