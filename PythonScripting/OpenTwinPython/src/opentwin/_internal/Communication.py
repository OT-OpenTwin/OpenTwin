# @otlicense

import ssl
import json

from typing import Any
from urllib.request import Request, urlopen
from urllib.error import HTTPError, URLError

from opentwin.return_message import ReturnMessage, ReturnMessageStatus
from opentwin._internal.ActionTypes import *
from opentwin.config import OpenTwinConfig

class Communication:
    """
    Central communication wrapper for OpenTwin services.
    
    This class is responsible for sending requests to the Global Session Service and handling responses.

    Attributes:
        globalSessionServiceUrl (str): The URL of the Global Session Service.
    """
    def __init__(self, globalSessionServiceUrl: str) -> None:
        self._initialized = False
        self._config = OpenTwinConfig()
        self._gssUrl = globalSessionServiceUrl

        # Initialize TLS configuration
        self._ssl_context = self._create_ssl_context()

        self._print(f"Initializing Communication with Global Session Service URL: {self._gssUrl}")

        # Create a request to the Global Session Service to retrieve the global information
        
        globalInfoRequest = {
            OT_ACTION_MEMBER: OT_ACTION_CMD_GetGlobalServicesUrl
        }
        globalInfoResponseMessage = self.send_to_return_message(self._gssUrl, globalInfoRequest)

        if not globalInfoResponseMessage.is_ok():
            raise RuntimeError("Failed to retrieve global information from the Global Session Service: " + globalInfoResponseMessage.what)

        globalInfoResponse = json.loads(globalInfoResponseMessage.what)

        # Process the global information
        if OT_ACTION_PARAM_SERVICE_DBURL in globalInfoResponse:
            self._databaseUrl = globalInfoResponse[OT_ACTION_PARAM_SERVICE_DBURL]
        else:
            raise RuntimeError("Global Session Service did not return a database URL")
        
        if OT_ACTION_PARAM_SERVICE_AUTHURL in globalInfoResponse:
            self._authUrl = globalInfoResponse[OT_ACTION_PARAM_SERVICE_AUTHURL]
        else:
            raise RuntimeError("Global Session Service did not return an authorization URL")
        
        if OT_ACTION_PARAM_SERVICE_GDSURL in globalInfoResponse:
            self._gdsUrl = globalInfoResponse[OT_ACTION_PARAM_SERVICE_GDSURL]
        else:
            raise RuntimeError("Global Session Service did not return a global directory URL")
        
        if OT_ACTION_PARAM_GlobalLoggerUrl in globalInfoResponse:
            self._loggerUrl = globalInfoResponse[OT_ACTION_PARAM_GlobalLoggerUrl]
        else:
            raise RuntimeError("Global Session Service did not return a logger URL")

        self._initialized = True
        self._print("Initialized Communication successfully with:")
        self._print(f"  Global Session Service URL: {self._gssUrl}" + " (connection confirmed)")
        self._print(f"  Database URL: {self._databaseUrl}")
        self._print(f"  Authorization URL: {self._authUrl}")
        self._print(f"  Global Directory URL: {self._gdsUrl}")
        self._print(f"  Logger URL: {self._loggerUrl}")

    @staticmethod
    def _normalize_url(url: str) -> str:
        """Ensure that a URL contains an HTTPS scheme.

        Args:
            url: URL to normalize.

        Returns:
            Normalized HTTPS URL.
        """
        if not url.startswith(("http://", "https://")):
            return f"https://{url}"

        return url

    ################################################################################################################################

    # Properties

    @property
    def global_session_service_url(self) -> str:
        return self._gssUrl

    @property
    def authentication_url(self) -> str:
        return self._authUrl

    @property
    def global_directory_url(self) -> str:
        return self._gdsUrl

    @property
    def gateway_url(self) -> str:
        return self._gatewayUrl
    
    @property
    def logger_url(self) -> str:
        return self._loggerUrl

    def is_initialized(self) -> bool:
        return self._initialized

    # ################################################################################################################################

    # General communication methods

    def _create_ssl_context(self) -> ssl.SSLContext:
        """Create an SSL context using the configured CA certificate.

        Returns:
            Configured SSL context.

        Raises:
            FileNotFoundError: If the CA certificate does not exist.
            ssl.SSLError: If the CA certificate cannot be loaded.
        """
        ca_path = self._config.ca_certificate_path

        if not ca_path.is_file():
            raise FileNotFoundError(f"CA certificate not found: {ca_path}")

        context = ssl.create_default_context(
            purpose=ssl.Purpose.SERVER_AUTH,
            cafile=str(ca_path)
        )

        return context

    def send_to_string(self, url: str, request_data: dict[str, Any]) -> str:
        """Send a JSON POST request and return the response body.

        Args:
            url: Destination service URL.
            request_data: Request data to serialize as JSON.

        Returns:
            Response body as a string.

        Raises:
            RuntimeError: If the server returns an HTTP error.
            ConnectionError: If the connection fails.
        """
        request_body = json.dumps(request_data).encode("utf-8")

        endpoint = self._normalize_url(url) + "/execute-one-way-tls"

        request = Request(
            url=endpoint,
            data=request_body,
            headers={
                "Content-Type": "application/json",
                "Accept": "application/json"
            },
            method="POST"
        )

        try:
            with urlopen(request,
                context=self._ssl_context,
                timeout=None # No timeout
            ) as response:
                return response.read().decode("utf-8")

        except HTTPError as error:
            raise RuntimeError(
                f"HTTP request failed with status code {error.code}"
            ) from error

        except URLError as error:
            raise ConnectionError(
                f"Could not connect to service: {error.reason}"
            ) from error

    def send_to_json(self, url: str, request_data: dict[str, Any]) -> dict[str, Any]:
        """Send a JSON request and deserialize the response.

        Args:
            url: Destination service URL.
            request_data: Request data to serialize as JSON.

        Returns:
            Deserialized JSON response.

        Raises:
            ValueError: If the response is invalid JSON or not an object.
        """
        response_body = self.send_to_string(url, request_data)

        try:
            response_data = json.loads(response_body)
        except json.JSONDecodeError as error:
            raise ValueError(
                "Failed to decode JSON response"
            ) from error

        if not isinstance(response_data, dict):
            raise ValueError(
                "Service returned an unexpected JSON structure"
            )

        return response_data

    def send_to_return_message(self, url: str, request_data: dict[str, Any]) -> ReturnMessage:
        """Send a JSON request and return a ReturnMessage object.

        Args:
            url: Destination service URL.
            request_data: Request data to serialize as JSON.
            execute: Whether to execute the request.

        Returns:
            A ReturnMessage object representing the response.

        Raises:
            RuntimeError: If the server returns an HTTP error.
            ConnectionError: If the connection fails.
        """
        try:
            response_data = self.send_to_json(url, request_data)
            return ReturnMessage.from_json(response_data)
        except Exception as e:
            return ReturnMessage(status=ReturnMessageStatus.StatusFailed, what=str(e))

    ################################################################################################################################
    
    @staticmethod
    def _print(message: str) -> None:
        print("[OpenTwin] [COMM] " + message)