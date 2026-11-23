# @otlicense

# Python standard
import logging
logger = logging.getLogger("OpenTwin")

# OpenTwin
from opentwin_api._internal.io.action_types import *
from opentwin_api._internal.io.gateway_action import GatewayAction
from opentwin_api._internal.io.gateway_communication import GatewayCommunication
from opentwin_api._internal.return_message import ReturnMessageStatus

class FrontendAPI:

    def __init__(self, gateway: GatewayCommunication) -> None:

        if not isinstance(gateway, GatewayCommunication):
            raise TypeError("gateway must be an instance of GatewayCommunication")
        self.gateway = gateway

    def press_menu_button(self,
                          button_name: str,
                          page_name: str,
                          group_name: str,
                          subgroup_name: str | None = None
                          ) -> None:
        
        if not isinstance(button_name, str):
            raise TypeError("button_name must be a string")
        if button_name == "":
            raise ValueError("button_name cannot be an empty string")
        if not isinstance(page_name, str):
            raise TypeError("page_name must be a string")
        if page_name == "":
            raise ValueError("page_name cannot be an empty string")
        if not isinstance(group_name, str):
            raise TypeError("group_name must be a string")
        if subgroup_name is not None and not isinstance(subgroup_name, str):
            raise TypeError("subgroup_name must be a string or None")

        # Send the action to the gateway
        request_data = {
            OT_ACTION_MEMBER: OT_ACTION_CMD_UI_ClickMenuButton,
            OT_ACTION_PARAM_UI_CONTROL_PageName: page_name,
            OT_ACTION_PARAM_UI_CONTROL_GroupName: group_name,
            OT_ACTION_PARAM_UI_CONTROL_SubgroupName: subgroup_name,
            OT_ACTION_PARAM_UI_CONTROL_ObjectName: button_name
        }

        logger.debug(f"Sending request to press menu button '{button_name}' on page '{page_name}', group '{group_name}', subgroup '{subgroup_name}'...")

        response = self.gateway.send_action(
            action=GatewayAction.QueueToService,
            receiver=OT_INFO_SERVICE_TYPE_UI,
            data=request_data
        )

        if response.status != ReturnMessageStatus.StatusOk:
            raise RuntimeError(f"Failed to press menu button '{button_name}': {response.what}")