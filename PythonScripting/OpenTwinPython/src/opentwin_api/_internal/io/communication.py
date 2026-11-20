# @otlicense

# Python standard
import ssl
import json
from pathlib import Path
from typing import Any
from typing import final
from typing_extensions import override
from urllib.request import Request, urlopen
from urllib.error import HTTPError, URLError
import logging
logger = logging.getLogger("OpenTwin")

# OpenTwin
from opentwin_api._internal.return_message import ReturnMessage, ReturnMessageStatus

# OpenTwin internal
from opentwin_api._internal.io.action_types import *
from opentwin_api._internal.io.communication_api import ICommunication
from opentwin_api._internal.service.gss_api import GSSAPI
from opentwin_api._internal.service.auth_api import AuthorizationServiceAPI

class Communication(ICommunication):
    
    def __init__(self, 
                 globalSessionServiceUrl: str,
                 ca_certificate_path: Path
                 ) -> None:
        
        self._initialized: bool = False
        
        # Initialize TLS configuration
        self._ssl_context = self._create_ssl_context(ca_certificate_path)

        logger.debug(f"Fetching global information from the Global Session Service at \"{globalSessionServiceUrl}\"...")

        # Create a request to the Global Session Service to retrieve the global information
        
        globalInfoRequest = {
            OT_ACTION_MEMBER: OT_ACTION_CMD_GetGlobalServicesUrl
        }
        globalInfoResponseMessage = self.send_to_return_message(globalSessionServiceUrl, globalInfoRequest)

        logger.debug(f"Global information: {globalInfoResponseMessage}")

        if globalInfoResponseMessage.status != ReturnMessageStatus.StatusOk:
            raise RuntimeError("Failed to retrieve global information from the Global Session Service: " + globalInfoResponseMessage.what)

        self._gss = GSSAPI(globalSessionServiceUrl, self)

        globalInfoResponse = json.loads(globalInfoResponseMessage.what)

        # Process the global information
        if OT_ACTION_PARAM_SERVICE_DBURL in globalInfoResponse:
            self._databaseUrl = globalInfoResponse[OT_ACTION_PARAM_SERVICE_DBURL]
        else:
            raise RuntimeError("Global Session Service did not return a database URL")
        
        if OT_ACTION_PARAM_SERVICE_AUTH_URL in globalInfoResponse:
            auth_url = globalInfoResponse[OT_ACTION_PARAM_SERVICE_AUTH_URL]
            self._auth = AuthorizationServiceAPI(auth_url, self)
        else:
            raise RuntimeError("Global Session Service did not return an authorization URL")
        
        if OT_ACTION_PARAM_SERVICE_GDS_URL in globalInfoResponse:
            self._gdsUrl = globalInfoResponse[OT_ACTION_PARAM_SERVICE_GDS_URL]
        else:
            raise RuntimeError("Global Session Service did not return a global directory URL")
        
        if OT_ACTION_PARAM_GlobalLoggerUrl in globalInfoResponse:
            self._loggerUrl = globalInfoResponse[OT_ACTION_PARAM_GlobalLoggerUrl]
        else:
            raise RuntimeError("Global Session Service did not return a logger URL")

        self._initialized = True
        logger.debug(__class__.__name__ + " initialized successfully")

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

    def getGSS(self) -> GSSAPI:
        return self._gss

    def getAuth(self) -> AuthorizationServiceAPI:
        return self._auth

    def is_initialized(self) -> bool:
        return self._initialized

    # ################################################################################################################################

    # General communication methods

    def _create_ssl_context(self, ca_certificate_path: Path) -> ssl.SSLContext:
        """Create an SSL context using the configured CA certificate.

        Returns:
            Configured SSL context.

        Raises:
            FileNotFoundError: If the CA certificate does not exist.
            ssl.SSLError: If the CA certificate cannot be loaded.
        """

        if not ca_certificate_path.is_file():
            raise FileNotFoundError(f"CA certificate not found: {ca_certificate_path}")

        context = ssl.create_default_context(
            purpose=ssl.Purpose.SERVER_AUTH,
            cafile=str(ca_certificate_path)
        )

        return context

    @override
    @final
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
        logger.debug(f"Sending request to {url}: {request_body.decode('utf-8')}")

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
                response_body = response.read().decode("utf-8")
                logger.debug(f"Received response from {url}: {response_body}")
                return response_body

        except HTTPError as error:
            raise RuntimeError(
                f"HTTP request failed with status code {error.code}"
            ) from error

        except URLError as error:
            raise ConnectionError(
                f"Could not connect to service: {error.reason}"
            ) from error

    @override
    @final
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

    @override
    @final
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
