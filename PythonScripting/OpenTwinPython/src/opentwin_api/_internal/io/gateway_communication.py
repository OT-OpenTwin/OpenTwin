# @otlicense

# Python standard
import logging
logger = logging.getLogger("OpenTwin")

# OpenTwin internal
from typing import Any

from opentwin_api._internal.io.action_types import *
from opentwin_api._internal.io.communication import Communication
from opentwin_api._internal.io.gateway_action import GatewayAction
from opentwin_api._internal.launching.gateway_connection_info import GatewayConnectionInfo
from opentwin_api._internal.return_message import ReturnMessage, ReturnMessageStatus

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
        self.info = connection_info
        self.communication = general_communication

        # Verify connection
        self.send_ping()

    def close(self) -> None:
        logger.debug("Closing gateway communication...")
        self.send_action(GatewayAction.Shutdown)

    def send_ping(self) -> None:
        response = self.send_action(GatewayAction.Ping)
        if response.status != ReturnMessageStatus.StatusOk:
            raise RuntimeError(f"Failed to ping gateway: {response.what}")
        if response.what != GatewayAction.Ping.value:
            raise RuntimeError(f"Unexpected response from gateway ping: {response.what}")

    def send_action(self, 
                    action: GatewayAction,
                    receiver: str | None = None,
                    data: dict[str, Any] | None = None
                    ) -> ReturnMessage:

        if not isinstance(action, GatewayAction):
            raise TypeError("action must be an instance of GatewayAction")

        action_request: dict[str, Any] = {
            OT_ACTION_MEMBER: OT_ACTION_CMD_PerformGatewayAction,
            OT_ACTION_PARAM_Ticket: self.info.gateway_ticket,
            OT_ACTION_PARAM_Gateway_Action: str(action)
        }
    
        if data is not None:
            action_request[OT_ACTION_PARAM_Data] = data

        if receiver is not None:
            action_request[OT_ACTION_PARAM_SERVICE_NAME] = receiver

        return self.communication.send_to_return_message(
            url=self.info.gateway_url,
            request_data=action_request
        )
