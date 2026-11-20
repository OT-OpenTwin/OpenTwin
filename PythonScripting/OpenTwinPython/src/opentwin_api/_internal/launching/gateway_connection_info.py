# @otlicense

class GatewayConnectionInfo:

    def __init__(self, 
                 gateway_url: str,
                 gateway_token: str) -> None:

        if not isinstance(gateway_url, str):
            raise TypeError("gateway_url must be a string")
        if gateway_url == "":
            raise ValueError("gateway_url must not be empty")
        self._gateway_url = gateway_url

        if not isinstance(gateway_token, str):
            raise TypeError("gateway_token must be a string")
        if gateway_token == "":
            raise ValueError("gateway_token must not be empty")
        self._gateway_token = gateway_token

    @property
    def gateway_url(self) -> str:
        return self._gateway_url

    @property
    def gateway_token(self) -> str:
        return self._gateway_token
    