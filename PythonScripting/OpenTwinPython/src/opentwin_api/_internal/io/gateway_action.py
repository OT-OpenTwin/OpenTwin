# @otlicense

from enum import Enum

class GatewayAction(Enum):
    Unknown = "Unknown"
    Ping = "Ping"
    Shutdown = "Shutdown"
    SendToService = "SendToService"
    QueueToService = "QueueToService"

    def __str__(self) -> str:
        return self.value