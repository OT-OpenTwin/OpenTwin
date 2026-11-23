# @otlicense

# OpenTwin internal
from opentwin_api._internal.io.communication import Communication
from opentwin_api._internal.launching.gateway_connection_info import GatewayConnectionInfo

class GatewayCommunication:

    def __init__(self,
                 connection_info: GatewayConnectionInfo,
                 general_communication: Communication
                 ) -> None:
        if not isinstance(connection_info, GatewayConnectionInfo):
            raise TypeError("connection_info must be an instance of GatewayConnectionInfo")

        if not isinstance(general_communication, Communication):
            raise TypeError("general_communication must be an instance of Communication")

        # Attempt to establish connection with the gateway
        self._gateway_url = connection_info.gateway_url

        
        

    @property
    def gateway_url(self) -> str:
        return self._gateway_url

    
