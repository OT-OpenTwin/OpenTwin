# @otlicense

# OpenTwin internal
from opentwin_api._internal.io.communication_api import ICommunication

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