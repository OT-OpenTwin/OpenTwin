# @otlicense

from opentwin_api._internal.io.gateway_communication import GatewayCommunication

class Project:

    def __init__(self, name: str, gateway_communication: GatewayCommunication | None = None) -> None:
        if not isinstance(name, str):
            raise TypeError("name must be a string")
        self._name = name

        #if not isinstance(gateway_communication, GatewayCommunication):
        #    raise TypeError("gateway_communication must be an instance of GatewayCommunication")
        #self._gateway_communication = gateway_communication

    @property
    def name(self) -> str:
        return self._name
