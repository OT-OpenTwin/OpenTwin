import json
from typing import Any

from enum import Enum

class ReturnMessageStatus(Enum):
    StatusOk = 0
    StatusFailed = 1
    StatusTrue = 2
    StatusFalse = 3
    StatusRetry = 4
    StatusCancel = 5

    def __str__(self) -> str:
        return self.name.removeprefix("Status")

    @classmethod
    def from_string(cls, value: str) -> "ReturnMessageStatus":
        try:
            return cls[f"Status{value}"]
        except KeyError:
            raise ValueError(
                f"Unknown ReturnMessageStatus: {value}"
            ) from None

        
class ReturnMessage:
    def __init__(self, status: ReturnMessageStatus = ReturnMessageStatus.StatusOk, what: str = ""):
        self._status = status
        self._what = what

    @property
    def status(self) -> ReturnMessageStatus:
        return self._status

    @property
    def what(self) -> str:
        return self._what

    @classmethod
    def from_json(cls, json_data:dict[str, Any]) -> "ReturnMessage":
        """Create a ReturnMessage from a JSON string.

        Args:
            json_data: JSON data to deserialize.
        Returns:
            A new ReturnMessage instance.
        """
        return cls(
            status=ReturnMessageStatus.from_string(json_data.get("Status", ReturnMessageStatus.StatusFailed)),
            what=json_data.get("What", "")
            )

    def to_json(self) -> str:
        return json.dumps({
            "Status": str(self._status),
            "What": self._what}
            )
