# @otlicense

import json
from typing import Any

# OpenTwin internal
from opentwin._internal.ActionTypes import *

class UiLaunchData:

    def __init__(self, 
                 ticket: str,
                 project: str,
                 gss_url: str
                 ) -> None:

        self._ticket: str = ticket
        self._project: str = project
        self._gss_url: str = gss_url

    @property
    def ticket(self) -> str:
        return self._ticket

    @property
    def project(self) -> str:
        return self._project

    @property
    def gss_url(self) -> str:
        return self._gss_url
        