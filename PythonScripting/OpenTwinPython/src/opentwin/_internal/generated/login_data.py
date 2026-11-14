# @otlicense

import json
from typing import Any

# OpenTwin internal
from opentwin._internal.ActionTypes import *

class LoginData:

    def __init__(self, 
                 original_username: str | None,
                 db_user: str | None,
                 db_password: str | None
                 ) -> None:
        
        self._original_user: str | None = original_username
        self._db_user: str | None = db_user
        self._db_password: str | None = db_password
        
    @property
    def db_user(self) -> str | None:
        return self._db_user

    @property
    def db_password(self) -> str | None:
        return self._db_password

    @property
    def original_user(self) -> str | None:
        return self._original_user

    @classmethod
    def from_json(cls, json_data:dict[str, Any]) -> "LoginData":
        return cls(
            original_username=json_data.get(OT_PARAM_AUTH_USERNAME),
            db_user=json_data.get(OT_PARAM_DB_USERNAME),
            db_password=json_data.get(OT_PARAM_DB_PASSWORD)
        )

    def to_json(self) -> str:
        return json.dumps({
            OT_PARAM_AUTH_USERNAME: self._original_user,
            OT_PARAM_DB_USERNAME: self._db_user, 
            OT_PARAM_DB_PASSWORD: self._db_password}
            )