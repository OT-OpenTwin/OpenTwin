# License:
# Copyright 2026 by OpenTwin
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""The admin panel: Apache from the Apache folder below the current directory, in a hidden window."""

import os
import subprocess
import sys

from .environment import Environment, services

APACHE = "Apache"
HTTPD = "\\bin\\httpd.exe"


def admin_panel(env: Environment) -> None:
    panel = dict(env)
    services(panel)
    panel["SRVROOT"] = os.getcwd() + "/" + APACHE

    # TODO(linux): httpd.exe and the hidden console window are Windows only.
    startup = subprocess.STARTUPINFO()
    startup.dwFlags |= subprocess.STARTF_USESHOWWINDOW
    startup.wShowWindow = subprocess.SW_HIDE
    sys.stdout.flush()
    try:
        subprocess.Popen([panel["SRVROOT"] + HTTPD], env=panel, startupinfo=startup,
                         creationflags=subprocess.CREATE_NEW_CONSOLE)
    except OSError as error:
        print(error, file=sys.stderr, flush=True)
