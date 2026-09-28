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

"""The opentwin:// URL protocol for the current user.

The only registry write in the rail: four values below SCHEME_KEY in
HKEY_CURRENT_USER. Nothing is ever deleted.
"""

import sys
from pathlib import Path
from typing import Mapping

from ..core import paths
from ..core.expansion import get
from ..core.platform import WINDOWS

# TODO(linux): the URL protocol is registered in the Windows registry.
if WINDOWS:
    import winreg

SCHEME_KEY = r"Software\Classes\OpenTwin"
SUBKEYS = ("", r"\DefaultIcon", r"\shell\open\command")
FRONTEND = paths.DEPLOYMENT + ("uiFrontend.exe",)


def _set_value(subkey: str, name: str, value: str) -> None:
    if subkey not in SUBKEYS or not SCHEME_KEY.endswith(r"\OpenTwin"):
        raise OSError(f"Refusing to write to {SCHEME_KEY}{subkey}")
    with winreg.CreateKeyEx(winreg.HKEY_CURRENT_USER, SCHEME_KEY + subkey, 0, winreg.KEY_SET_VALUE) as handle:
        winreg.SetValueEx(handle, name, 0, winreg.REG_SZ, value)
    print("The operation completed successfully.", flush=True)


def register_scheme(env: Mapping[str, str]) -> int:
    print("Registering OpenTwin URL protocol...", flush=True)
    frontend = Path(get(env, "OPENTWIN_DEV_ROOT")).joinpath(*FRONTEND)
    if not frontend.exists():
        print("ERROR: OpenTwin executable not found:", flush=True)
        print(f'       "{frontend}"', flush=True)
        return 1

    print("Executable:")
    print(f'  "{frontend}"')
    print(flush=True)

    try:
        _set_value("", "", "URL:OpenTwin Protocol")
        _set_value("", "URL Protocol", "")
        _set_value(r"\DefaultIcon", "", f'"{frontend}",0')
        _set_value(r"\shell\open\command", "", f'"{frontend}" "%1"')
    except OSError as error:
        print(error, file=sys.stderr)
        print()
        print("ERROR: Failed to register OpenTwin URL protocol.", flush=True)
        return 1

    print()
    print("Successfully registered:")
    print("  opentwin://")
    print()
    print("Command:")
    print(f'  "{frontend}" "%1"')
    print()
    print("You can test it with:")
    print('  start "" "opentwin://open?project=test"', flush=True)
    return 0
