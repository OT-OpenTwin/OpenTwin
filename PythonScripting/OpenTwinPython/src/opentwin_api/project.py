# @otlicense

from opentwin_api._internal.io.gateway_communication import GatewayCommunication

class Project:

    def __init__(self,
                 name: str,
                 id: str,
                 gateway: GatewayCommunication | None = None
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

    @property
    def name(self) -> str:
        return self._name

    @property
    def id(self) -> str:
        return self._id
