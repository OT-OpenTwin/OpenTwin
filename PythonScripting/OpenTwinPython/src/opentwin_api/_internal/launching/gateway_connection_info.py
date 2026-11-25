# @otlicense

class GatewayConnectionInfo:

    def __init__(self, 
                 gateway_url: str,
                 gateway_ticket: str) -> None:

        if not isinstance(gateway_url, str):
            raise TypeError("gateway_url must be a string")
        if gateway_url == "":
            raise ValueError("gateway_url must not be empty")
        self._gateway_url = gateway_url

        if not isinstance(gateway_ticket, str):
            raise TypeError("gateway_ticket must be a string")
        if gateway_ticket == "":
            raise ValueError("gateway_ticket must not be empty")
        self._gateway_ticket = gateway_ticket

    @property
    def gateway_url(self) -> str:
        return self._gateway_url

    @property
    def gateway_ticket(self) -> str:
        return self._gateway_ticket
    