# @otlicense

# OpenTwin internal
from opentwin_api._internal.auth.user_session_token import UserSessionToken
from opentwin_api._internal.io.action_types import *
from opentwin_api._internal.io.communication_api import ICommunication
from opentwin_api._internal.launching.gateway_connection_info import GatewayConnectionInfo
from opentwin_api._internal.return_message import ReturnMessage, ReturnMessageStatus

class LSSAPI:
    
    def __init__(self, url: str, communication: ICommunication):
        if not isinstance(url, str):
            raise TypeError("url must be a string")
        if url == "":
            raise ValueError("url must not be empty")
        self._url = url

        if not isinstance(communication, ICommunication):
            raise TypeError("communication must be an instance of ICommunication")
        self.communication = communication

    @property
    def url(self) -> str:
        return self._url

    def check_project_opened(self,
                             project_id: str
                             ) -> bool:

        """
        Check if a project is opened in the LSS (Local Session Service). 

        Args:
            project_id (str): Project identifier of the project to check.

        Returns:
            bool: True if the project is opened, False otherwise.

        Raises:
            TypeError: If an invalid argument type is provided.
            ValueError: If an invalid argument value is provided.

        """
        
        if not isinstance(project_id, str):
            raise TypeError("project_id must be a string")

        if project_id == "":
            raise ValueError("project_id must not be empty")

        request_document = {
            OT_ACTION_MEMBER: OT_ACTION_CMD_CheckSessionRunning,
            OT_ACTION_PARAM_SESSION_ID: project_id
        }
        
        response_message: ReturnMessage = self.communication.send_to_return_message(self._url, request_document)
        return response_message.status == ReturnMessageStatus.StatusTrue
    
    def request_gateway_launch(self,
                               project_id: str,
                               gateway_launch_ticket: str,
                               user_session_token: UserSessionToken
                               ) -> GatewayConnectionInfo:

        if not isinstance(project_id, str):
            raise TypeError("project_id must be a string")
        if project_id == "":
            raise ValueError("project_id must not be empty")

        if not isinstance(gateway_launch_ticket, str):
            raise TypeError("gateway_launch_ticket must be a string")
        if gateway_launch_ticket == "":
            raise ValueError("gateway_launch_ticket must not be empty")

        if not isinstance(user_session_token, UserSessionToken):
            raise TypeError("user_session_token must be an instance of UserSessionToken")

        request_document = {
            OT_ACTION_MEMBER: OT_ACTION_CMD_StartNewGateway,
            OT_ACTION_PARAM_Project: project_id,
            OT_ACTION_PARAM_Ticket: gateway_launch_ticket,
            OT_ACTION_PARAM_SessionToken: user_session_token.to_json()
        }
