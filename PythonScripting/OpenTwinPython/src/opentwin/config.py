# @otlicense

import os
from pathlib import Path
from opentwin._internal.system.DirectoryManager import DirectoryManager

class OpenTwinConfig:
    """Central configuration for the OpenTwin Python module.

    Implements a singleton to ensure that all internal components
    use the same configuration and file paths.
    """

    _instance = None

    def __new__(cls):
        """Create or return the singleton instance."""
        if cls._instance is None:
            cls._instance = super().__new__(cls)

        return cls._instance

    def __init__(self) -> None:
        """Initialize configuration paths once."""
        if hasattr(self, "_initialized"):
            return

        self._initialized = True

        # Read certificate directory from environment
        certDir = os.environ.get("OPEN_TWIN_CERTS_PATH")
        certDir = Path(certDir) if certDir else None

        if certDir is not None:
            self._ca_certificate_path = certDir / "ca.pem"

    def invariant(self) -> None:
        if not hasattr(self, "_ca_certificate_path"):
            raise RuntimeError("CA certificate path is not set. Please set OPEN_TWIN_CERTS_PATH environment variable.")

        if not isinstance(self._ca_certificate_path, Path):
            raise TypeError("CA certificate path must be a pathlib.Path object.")

        if not self._ca_certificate_path.exists():
            raise FileNotFoundError(f"CA certificate file not found at {self._ca_certificate_path}. Please ensure the file exists.")

    @property
    def ca_certificate_path(self) -> Path:
        return self._ca_certificate_path
