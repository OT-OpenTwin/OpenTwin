import json

from typing import Any
from urllib.request import Request, urlopen
from urllib.error import HTTPError, URLError

class Communication:
    """
    Central communication wrapper for OpenTwin services.
    
    This class is responsible for sending requests to the Global Session Service and handling responses.

    Attributes:
        globalSessionServiceUrl (str): The URL of the Global Session Service.
        messageTimeout (int): The timeout for each message in seconds.
    """
    def __init__(self, globalSessionServiceUrl: str, messageTimeout: int) -> None:
        self._gssUrl = globalSessionServiceUrl
        self._messageTimeout = messageTimeout
        self._authUrl = None  # Placeholder for future authentication URL
        self._lssUrl = None  # Placeholder for future local session service URL
        self._gatewayUrl = None  # Placeholder for future gateway URL

    ################################################################################################################################

    # Properties

    @property
    def global_session_service_url(self) -> str:
        return self._gssUrl

    @property
    def authentication_url(self) -> str:
        return self._authUrl

    @property
    def session_service_url(self) -> str:
        return self._lssUrl

    @property
    def gateway_url(self) -> str:
        return self._gatewayUrl
    
    # ################################################################################################################################

    # General communication methods

    def init(self) -> None:
        # 

    """
    Send a request to the specified URL and return the response as a string.

    Attributes:
        url (str): The URL to send the request to.
        request_data (dict[str, Any]): The data to be sent in the request body.
    
    Returns:
        str: The response body as a string.

    Raises:
        RuntimeError: If the HTTP request fails with a non-200 status code.
    """
    def sendTo(self, url: str, request_data: dict[str, Any]) -> str:
        # Serialize request data
        request_body = json.dumps(request_data).encode("utf-8")

        # Construct HTTP request
        request = Request(url=url, data=request_body,
            headers={
                "Content-Type": "application/json",
                "Accept": "application/json"
            },
            method="POST"
        )

        # Send request and receive response
        try:
            with urlopen(request, timeout=self._messageTimeout) as response:
                response_body = response.read().decode("utf-8")

        except HTTPError as error:
            raise RuntimeError(f"HTTP request failed with status code {error.code}") from error

        except URLError as error:
            raise ConnectionError(f"Could not connect to service: {error.reason}") from error

        return response_body


    """
    Send a request to the specified URL and return the response as a json dictionary.
    """
    def sendTo2(self, url: str, request_data: dict[str, Any]) -> Any:
        # Send request and receive response
        response_body = self.sendTo(url, request_data)

        # Deserialize response data
        try:
            response_data = json.loads(response_body)
        except json.JSONDecodeError as error:
            raise ValueError("Failed to decode JSON response") from error

        if not isinstance(response_data, dict):
            raise ValueError("Service returned an unexpected JSON structure")

        return response_data

    ################################################################################################################################
    