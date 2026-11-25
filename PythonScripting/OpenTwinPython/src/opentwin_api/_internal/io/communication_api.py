# @otlicense

# Python standard
from typing import Any
from abc import ABC, abstractmethod

# OpenTwin internal
from opentwin_api._internal.return_message import ReturnMessage

class ICommunication(ABC):

    @abstractmethod
    def send_to_string(self, url: str, request_data: dict[str, Any]) -> str:
        raise NotImplementedError("send_to_string method must be implemented by subclasses")

    @abstractmethod
    def send_to_json(self, url: str, request_data: dict[str, Any]) -> dict[str, Any]:
        raise NotImplementedError("send_to_json method must be implemented by subclasses")

    @abstractmethod
    def send_to_return_message(self, url: str, request_data: dict[str, Any]) -> ReturnMessage:
        raise NotImplementedError("send_to_return_message method must be implemented by subclasses")
    