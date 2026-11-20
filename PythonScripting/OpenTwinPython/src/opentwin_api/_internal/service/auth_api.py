# @otlicense

# Python standard
import json
from typing import Any
import logging
logger = logging.getLogger("OpenTwin")

from opentwin_api._internal.auth.user_session_token import UserSessionToken

# OpenTwin internal
from opentwin_api._internal.io.action_types import *
from opentwin_api._internal.crypto.global_crypto_key_store import GlobalCryptoKeyStore
from opentwin_api._internal.crypto.global_crypto_key_ids import *
from opentwin_api._internal.io.communication_api import ICommunication
from opentwin_api._internal.launching.launch_tickets import LaunchTickets
from opentwin_api._internal.return_message import ReturnMessage, ReturnMessageStatus
from opentwin_api._internal.system.encoding import Encoding

class AuthorizationServiceAPI:

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

    def login(self, login_token: str) -> UserSessionToken:
        if not isinstance(login_token, str):
            raise TypeError("login_token must be a string")
        
        # Create challenge request
        challengeRequestDocument = {
            OT_ACTION_MEMBER: OT_ACTION_LOGIN,
            OT_ACTION_PARAM_Token: login_token
        }
        
        logger.debug("Sending initial login request...")
        
        # Send challenge request
        challengeDoc: dict[str, Any] = self.communication.send_to_json(self._url, challengeRequestDocument)
        
        # Read challenge
        challengeID = challengeDoc[OT_ACTION_PARAM_ChallengeID]
        challenge = challengeDoc[OT_ACTION_PARAM_Challenge]
        
        if not challengeID or not challenge:
            raise RuntimeError("Failed to retrieve challenge from the Authorization Service")
        
        if not isinstance(challengeID, str) or not isinstance(challenge, str):
            raise RuntimeError("Invalid challenge response from the Authorization Service")
        
        challengeBytes = challenge.encode("utf-8")
        
        logger.debug(f"Signing challenge: {challenge}")
        
        # Sign the challenge with the global key store
        store = GlobalCryptoKeyStore.get()
        signature = store.sign(OT_CRYPTO_GLOBAL_KEY_ID_LOCAL_TOKEN_SIGNATURE, challengeBytes)
        
        logger.debug(f"Generated signature: {signature}")
        
        # Convert signature to base 64 url safe string
        signature = Encoding.toBase64Url(signature)
        
        # Create login request
        loginRequestDocument = {
            OT_ACTION_MEMBER: OT_ACTION_LOGIN,
            OT_ACTION_PARAM_Token: login_token,
            OT_ACTION_PARAM_ChallengeID: challengeID,
            OT_ACTION_PARAM_Signature: signature
        }
        
        # Send login request
        loginResponseDoc: ReturnMessage = self.communication.send_to_json(self._url, loginRequestDocument)

        # Read login response
        auth_success: bool = loginResponseDoc[OT_ACTION_AUTH_SUCCESS]
        if not isinstance(auth_success, bool):
            raise RuntimeError("Invalid login response from the Authorization Service")

        if not auth_success:
            raise RuntimeError("Login failed")

        userSessionTokenObj = loginResponseDoc[OT_ACTION_PARAM_SessionToken]

        session_token = userSessionTokenObj[OT_ACTION_PARAM_SessionToken]
        if not isinstance(session_token, str):
            raise RuntimeError("Invalid session token received from the Authorization Service")

        session_name = userSessionTokenObj[OT_ACTION_PARAM_SessionName]
        if not isinstance(session_name, str):
            raise RuntimeError("Invalid session name received from the Authorization Service")
        
        user_name = userSessionTokenObj[OT_ACTION_PARAM_Username]
        if not isinstance(user_name, str):
            raise RuntimeError("Invalid user name received from the Authorization Service")

        return UserSessionToken(
            token=session_token,
            session_name=session_name,
            user_name=user_name)


    def generate_launch_tickets(self, user_session_token: UserSessionToken) -> LaunchTickets:
        if not isinstance(user_session_token, UserSessionToken):
            raise TypeError("user_session_token must be a UserSessionToken instance")
    
        # Define the required tickets
        requiredTickets = [OT_TICKET_TYPE_Login,
                           OT_TICKET_TYPE_Gateway
                           ]
    
        # Create ticket request
        ticketRequestDocument = {
            OT_ACTION_MEMBER: OT_ACTION_CMD_GenerateTickets,
            OT_ACTION_PARAM_SessionToken: user_session_token.to_json(),
            OT_ACTION_PARAM_TicketType: requiredTickets
        }
    
        # Send the ticket request
        ticketResponseMessage = self.communication.send_to_return_message(self._url, ticketRequestDocument)
    
        # Validate the response
        if ticketResponseMessage.status != ReturnMessageStatus.StatusOk:
            logger.error(f"Failed to generate tickets. Response: {ticketResponseMessage.what}")
            raise RuntimeError("Failed to generate tickets")
    
        # Parse the ticket response
        ticketResponseDoc = json.loads(ticketResponseMessage.what)
    
        # Extract the required tickets
        ui_launch_ticket : str | None = None
        gateway_launch_ticket : str | None = None
    
        for ticketType in requiredTickets:
            if ticketType not in ticketResponseDoc:
                raise RuntimeError(f"Failed to retrieve ticket ({ticketType}) from the Global Session Service")
            if not isinstance(ticketResponseDoc[ticketType], list) or not all(isinstance(ticket, str) for ticket in ticketResponseDoc[ticketType]):
                raise RuntimeError(f"Invalid ticket ({ticketType}) response from the Global Session Service")
            if len(ticketResponseDoc[ticketType]) != 1:
                raise RuntimeError(f"Expected exactly one ticket for type ({ticketType}), but got {len(ticketResponseDoc[ticketType])}")
    
            ticket = ticketResponseDoc[ticketType][0]
    
            if ticketType == OT_TICKET_TYPE_Login:
                ui_launch_ticket = ticket
            elif ticketType == OT_TICKET_TYPE_Gateway:
                gateway_launch_ticket = ticket
            else:
                raise RuntimeError(f"Unexpected ticket type received: {ticketType}")
    
        # Ensure that all required tickets were retrieved
        if ui_launch_ticket is None or gateway_launch_ticket is None:
            raise RuntimeError("Failed to retrieve all required tickets from the Global Session Service")
    
        logger.debug(f"Successfully generated tickets: UI Launch Ticket: {ui_launch_ticket}, Gateway Launch Ticket: {gateway_launch_ticket}")
    
        # Return the launch tickets
        return LaunchTickets(
            ui_launch_ticket=ui_launch_ticket,
            gateway_launch_ticket=gateway_launch_ticket
        )

    def get_project_collection_from_name(self, user_session_token: UserSessionToken, project_name: str) -> str:
        if not isinstance(user_session_token, UserSessionToken):
            raise TypeError("user_session_token must be a UserSessionToken instance")
        if not isinstance(project_name, str):
            raise TypeError("project_name must be a string")
        
        # Create request to get project information
        projectInfoRequestDocument = {
            OT_ACTION_MEMBER: OT_ACTION_GET_PROJECT_DATA,
            OT_ACTION_PARAM_SessionToken: user_session_token.to_json(),
            OT_PARAM_AUTH_PROJECT_NAME: project_name
        }
        
        # Send request to the Authorization Service
        projectInfoResponseMessage = self.communication.send_to_return_message(self._url, projectInfoRequestDocument)
        
        # Validate the response
        if projectInfoResponseMessage.status != ReturnMessageStatus.StatusOk:
            logger.error(f"Failed to retrieve project information. Response: {projectInfoResponseMessage.what}")
            raise RuntimeError("Failed to retrieve project information")
        
        # Parse the project information response
        projectInfoResponseDoc = json.loads(projectInfoResponseMessage.what)
        
        # Extract the project collection
        return projectInfoResponseDoc["Collection"]