# @otlicense

# OpenTwin internal
from opentwin_api._internal.io import communication
from opentwin_api._internal.launching.gateway_connection_info import GatewayConnectionInfo

class GatewayCommunication:

    def __init__(self,
                 connection_info: GatewayConnectionInfo
                 ) -> None:
        if not isinstance(connection_info, GatewayConnectionInfo):
            raise TypeError("connection_info must be an instance of GatewayConnectionInfo")

        # Attempt to establish connection with the gateway
        self._gateway_url = connection_info.gateway_url

        

    @property
    def gateway_url(self) -> str:
        return self._gateway_url

    
