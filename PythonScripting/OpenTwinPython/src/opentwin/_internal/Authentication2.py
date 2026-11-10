# @otlicense

import base64
from typing import Any

from opentwin._internal.ActionTypes import *
from opentwin._internal.Communication2 import Communication
from opentwin._internal.crypto.GlobalCryptoKeyStore import GlobalCryptoKeyStore
from opentwin._internal.crypto.GlobalCryptoKeyIDs import *
from opentwin._internal.system.Encoding import Encoding

class Authentication:

    """
    A class for handling authentication with the OpenTwin services.

    This class provides methods for logging in and managing authentication tokens.

    Attributes:
        _io (Communication): The communication instance used to interact with the OpenTwin services.
        _login_token (str): The login token for authentication.
    """
    def __init__(self, communication: Communication, login_token: str) -> None:
        self._io = communication
        self._login_token = login_token

        challengeRequestDocument = {
            OT_ACTION_MEMBER: OT_ACTION_LOGIN,
            OT_ACTION_PARAM_Token: self._login_token
        }

        self._print("Sending initial login request...")

        challengeDoc = self._io.send_to_json(self._io.authentication_url, challengeRequestDocument)
        challengeID = challengeDoc[OT_ACTION_PARAM_ChallengeID]
        challenge = challengeDoc[OT_ACTION_PARAM_Challenge]

        if not challengeID or not challenge:
            raise RuntimeError("Failed to retrieve challenge from the Global Session Service")

        if not isinstance(challengeID, str) or not isinstance(challenge, str):
            raise RuntimeError("Invalid challenge response from the Global Session Service")

        challengeBytes = challenge.encode("utf-8")

        self._print(f"Signing challenge: {challenge}")

        # Sign the challenge with the global key store
        store = GlobalCryptoKeyStore.get()
        signature = store.sign(OT_CRYPTO_GLOBAL_KEY_ID_LOCAL_TOKEN_SIGNATURE, challengeBytes)

        self._print(f"Sending login request with signed challenge...")

        # Convert signature to base 64 url safe string
        signature = Encoding.toBase64Url(signature)

        loginRequestDocument = {
            OT_ACTION_MEMBER: OT_ACTION_LOGIN,
            OT_ACTION_PARAM_Token: self._login_token,
            OT_ACTION_PARAM_ChallengeID: challengeID,
            OT_ACTION_PARAM_Signature: signature
        }

        # Send the login request
        loginResponseDoc = self._io.send_to_json(self._io.authentication_url, loginRequestDocument)

        success = loginResponseDoc[OT_ACTION_AUTH_SUCCESS]
 
        if not success:
            print(f"Authentication failed. Response: {loginResponseDoc}")
            raise RuntimeError("Authentication failed")
        
        dbUser = loginResponseDoc[OT_PARAM_DB_USERNAME]
        dbPassword = loginResponseDoc[OT_PARAM_DB_PASSWORD]

        self._print(f"Authenticated successfully.")

    ################################################################################################################################

    @staticmethod
    def _print(message: str) -> None:
        print("[OpenTwin] [AUTH] " + message)