# @otlicense

# Python standard
import json
import logging
logger = logging.getLogger("OpenTwin")

# OpenTwin internal
from opentwin_api._internal.io.action_types import *
from opentwin_api._internal.io.communication_api import ICommunication
from opentwin_api._internal.return_message import ReturnMessageStatus

class GSSAPI:
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

    def determine_lss_for_project(self, project_id: str) -> str | None:
        if not isinstance(project_id, str):
            raise TypeError("project_id must be a string")

        # Create request to check if the project is open
        checkProjectRequestDocument: dict[str, str] = {
            OT_ACTION_MEMBER: OT_ACTION_CMD_IsProjectOpen,
            OT_ACTION_PARAM_PROJECT_NAME: project_id
        }

        # Send request to the Global Session Service
        checkProjectResponseMessage = self.communication.send_to_return_message(self._url, checkProjectRequestDocument)

        if checkProjectResponseMessage.status == ReturnMessageStatus.StatusTrue:           
            infoDoc = json.loads(checkProjectResponseMessage.what)
            return infoDoc[OT_ACTION_PARAM_LSS_URL]
        elif checkProjectResponseMessage.status == ReturnMessageStatus.StatusFalse:
            return None
        else:
            raise RuntimeError(f"Failed to check if project is open. Response: {checkProjectResponseMessage.what}")
