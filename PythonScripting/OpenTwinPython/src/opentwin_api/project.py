# @otlicense

# OpenTwin internal
from opentwin_api._internal.io.gateway_communication import GatewayCommunication
from opentwin_api.frontend_api import FrontendAPI

class Project:

    def __init__(self,
                 name: str,
                 id: str,
                 gateway: GatewayCommunication
                 ) -> None:
        
        if not isinstance(name, str):
            raise TypeError("name must be a string")
        self._name = name

        if not isinstance(id, str):
            raise TypeError("id must be a string")
        self._id = id

        if not isinstance(gateway, GatewayCommunication):
            raise TypeError("gateway must be an instance of GatewayCommunication")
        self.gateway = gateway

        self.frontend = FrontendAPI(self.gateway)

    @property
    def name(self) -> str:
        return self._name

    @property
    def id(self) -> str:
        return self._id

    def get_gateway_communication(self) -> GatewayCommunication:
        return self.gateway

    def get_frontend(self) -> FrontendAPI:
        return self.frontend

    def close(self) -> None:
        self.gateway.close()
