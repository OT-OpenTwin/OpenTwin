
class LaunchTickets:

    def __init__(self, ui_launch_ticket: str, gateway_launch_ticket: str) -> None:
        self._ui_launch_ticket = ui_launch_ticket
        self._gateway_launch_ticket = gateway_launch_ticket

    @property
    def ui_launch_ticket(self) -> str:
        return self._ui_launch_ticket

    @property
    def gateway_launch_ticket(self) -> str:
        return self._gateway_launch_ticket