# @otlicense

# OpenTwin internal
from opentwin_api._internal.ActionTypes import *

class UserSessionToken:

    def __init__(self,
                 token: str,
                 session_name: str,
                 user_name: str) -> None:

        if not isinstance(token, str):
            raise TypeError("token must be a string")
        if token == "":
            raise ValueError("token must not be empty")
        self._token = token

        if not isinstance(session_name, str):
            raise TypeError("session_name must be a string")
        if session_name == "":
            raise ValueError("session_name must not be empty")
        self._session_name = session_name

        if not isinstance(user_name, str):
            raise TypeError("user_name must be a string")
        if user_name == "":
            raise ValueError("user_name must not be empty")
        self._user_name = user_name

    @property
    def token(self) -> str:
        return self._token

    @property
    def session_name(self) -> str:
        return self._session_name

    @property
    def user_name(self) -> str:
        return self._user_name

    def to_json(self) -> dict[str, str]:
        return {
            OT_ACTION_PARAM_SessionToken: self._token,
            OT_ACTION_PARAM_SessionName: self._session_name,
            OT_ACTION_PARAM_Username: self._user_name
        }
        

