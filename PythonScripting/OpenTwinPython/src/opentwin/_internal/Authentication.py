# @otlicense

# Python standard
import json
from typing import Any
import logging
logger = logging.getLogger("OpenTwin")

# OpenTwin internal
from opentwin._internal.ActionTypes import *
from opentwin._internal.Communication import Communication
from opentwin._internal.crypto.GlobalCryptoKeyStore import GlobalCryptoKeyStore
from opentwin._internal.crypto.GlobalCryptoKeyIDs import *
from opentwin._internal.system.Encoding import Encoding
from opentwin._internal.return_message import ReturnMessage, ReturnMessageStatus
from opentwin._internal.launching.LaunchTickets import LaunchTickets

class Authentication:

    """
    A class for handling authentication with the OpenTwin services.

    This class provides methods for logging in and managing authentication tokens.

    Attributes:
        _io (Communication): The communication instance used to interact with the OpenTwin services.
        _login_token (str): The login token for authentication.
    """
    @classmethod
    def generate_launch_tickets(cls, communication: Communication, login_token: str) -> LaunchTickets:

        challengeRequestDocument = {
            OT_ACTION_MEMBER: OT_ACTION_CMD_GenerateTickets,
            OT_ACTION_PARAM_Token: login_token
        }

        logger.debug("Sending initial ticket generation request...")

        challengeResponseMessage: ReturnMessage = communication.send_to_return_message(communication.authentication_url, challengeRequestDocument)
        if challengeResponseMessage.status != ReturnMessageStatus.StatusOk:
            logger.error(f"Failed to generate tickets. Response: {challengeResponseMessage.what}")
            raise RuntimeError("Failed to generate tickets")
        
        challengeDoc = json.loads(challengeResponseMessage.what)
        challengeID = challengeDoc[OT_ACTION_PARAM_ChallengeID]
        challenge = challengeDoc[OT_ACTION_PARAM_Challenge]

        if not challengeID or not challenge:
            raise RuntimeError("Failed to retrieve challenge from the Global Session Service")

        if not isinstance(challengeID, str) or not isinstance(challenge, str):
            raise RuntimeError("Invalid challenge response from the Global Session Service")

        challengeBytes = challenge.encode("utf-8")

        logger.debug(f"Signing challenge: {challenge}")

        # Sign the challenge with the global key store
        store = GlobalCryptoKeyStore.get()
        signature = store.sign(OT_CRYPTO_GLOBAL_KEY_ID_LOCAL_TOKEN_SIGNATURE, challengeBytes)

        logger.debug(f"Generated signature: {signature}")

        # Convert signature to base 64 url safe string
        signature = Encoding.toBase64Url(signature)

        requiredTickets = [OT_TICKET_TYPE_Login,
                           OT_TICKET_TYPE_Gateway
                           ]

        ticketRequestDocument = {
            OT_ACTION_MEMBER: OT_ACTION_CMD_GenerateTickets,
            OT_ACTION_PARAM_Token: login_token,
            OT_ACTION_PARAM_ChallengeID: challengeID,
            OT_ACTION_PARAM_Signature: signature,
            OT_ACTION_PARAM_TicketType: requiredTickets
        }

        # Send the ticket request
        ticketResponseMessage = communication.send_to_return_message(communication.authentication_url, ticketRequestDocument)

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
