# @otlicense

import json
from typing import Any

# OpenTwin internal
from opentwin_api._internal.ActionTypes import *

class UiLaunchData:

    def __init__(self, 
                 ticket: str,
                 project_name: str,
                 project_version: str | None,
                 gss_url: str
                 ) -> None:

        if not isinstance(ticket, str):
            raise TypeError("ticket must be a string")
        self._ticket: str = ticket

        if not isinstance(project_name, str):
            raise TypeError("project_name must be a string")
        self._project_name: str = project_name

        if project_version is not None and not isinstance(project_version, str):
            raise TypeError("project_version must be a string or None")
        self._project_version: str | None = project_version

        if not isinstance(gss_url, str):
            raise TypeError("gss_url must be a string")
        self._gss_url: str = gss_url

    @property
    def ticket(self) -> str:
        return self._ticket

    @property
    def project_name(self) -> str:
        return self._project_name

    @property
    def project_version(self) -> str | None:
        return self._project_version

    @property
    def gss_url(self) -> str:
        return self._gss_url
        