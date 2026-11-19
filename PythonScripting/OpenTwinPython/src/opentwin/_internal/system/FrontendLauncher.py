# @otlicense

# Python standard
import base64
import json
import os
import sys
import subprocess
from pathlib import Path

import logging
logger = logging.getLogger("OpenTwin")

from opentwin._internal.ActionTypes import *
from opentwin._internal.launching.ui_launch_data import UiLaunchData
from opentwin._internal.system.Encoding import Encoding

class FrontendLauncher:
    
    def __init__(self, 
                 frontend_executable_path: Path
                 ) -> None:
        
        self._frontend_executable_path = Path(frontend_executable_path).resolve()

        logger.debug(f"Initializing " + __class__.__name__ + " with executable path \"{self._frontend_executable_path}\"...")

        if not self._frontend_executable_path.is_file():
            raise FileNotFoundError(f"Frontend executable not found: {self._frontend_executable_path}")

        if sys.platform != "win32" and not os.access(self._frontend_executable_path, os.X_OK):
            raise PermissionError(f"Frontend executable is not executable: {self._frontend_executable_path}")

        logger.debug(__class__.__name__ + " initialized successfully")

    def launch(self, launch_data: UiLaunchData) -> None:
        login_cfg_object = {
            OT_ACTION_PARAM_Ticket: launch_data.ticket,
            OT_ACTION_PARAM_SERVICE_GSS_URL: launch_data.gss_url
        }

        login_cfg_json = json.dumps(login_cfg_object)
        login_cfg_base64 : str = Encoding.toBase64Url(login_cfg_json.encode("utf-8"))
        
        args = [
            "--login-ticket", login_cfg_base64
        ]
    
        if not isinstance(args, list) or not all(isinstance(arg, str) for arg in args):
            raise TypeError("args must be a list of strings")

        command = [str(self._frontend_executable_path), *args]

        logger.debug(f"Launching OpenTwin frontend with command: {command}")
        return
        try:
            if sys.platform == "win32":
                subprocess.Popen(
                    command,
                    creationflags=(subprocess.DETACHED_PROCESS 
                                   | subprocess.CREATE_NEW_PROCESS_GROUP
                    ),
                    close_fds=True,
                    cwd=self._frontend_executable_path.parent,
                )

            elif sys.platform in ("linux", "darwin"):
                subprocess.Popen(
                    command,
                    start_new_session=True,
                    close_fds=True,
                    cwd=self._frontend_executable_path.parent,
                )

            else:
                raise NotImplementedError(f"Unsupported operating system: {sys.platform}")

        except OSError as exc:
            raise RuntimeError(f"Failed to launch OpenTwin frontend: {self._frontend_executable_path}") from exc
