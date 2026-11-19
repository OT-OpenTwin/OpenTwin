# @otlicense

from opentwin_api._internal.io import communication

class GatewayCommunication:

    def __init__(self, general_communication: communication, gateway_url: str) -> None:
        self._general_communication = general_communication
        self._gateway_url = gateway_url

    @property
    def gateway_url(self) -> str:
        return self._gateway_url

    @classmethod
    def try_connect(cls,
                    project_name: str,
                    gateway_ticket: str,
                    communication: communication) -> 'GatewayCommunication':
        if not isinstance(project_name, str):
            raise TypeError("project_name must be a string")
        if not isinstance(gateway_ticket, str):
            raise TypeError("gateway_ticket must be a string")
        if not isinstance(communication, communication):
            raise TypeError("communication must be an instance of Communication")

        newGateway = GatewayCommunication(communication, gateway_ticket)
        return newGateway