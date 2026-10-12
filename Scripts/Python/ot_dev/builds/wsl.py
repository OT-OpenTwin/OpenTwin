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

import os
import shutil
import subprocess
import sys
from typing import Sequence

from ..core.config import definitions
from ..core.environment import wslenv
from ..core.platform import WINDOWS

ARGUMENT = "WSL"

# Inside WSL, build.py runs as on a native Linux machine and reads SetupEnvironment.py there.
# -B: no __pycache__ in the Windows checkout
COMMAND = 'echo "Building in WSL: $WSL_DISTRO_NAME"; exec python3 -B "$OPENTWIN_DEV_ROOT/Scripts/Python/build.py" "$@"'


def build_in_wsl(argv: Sequence[str]) -> int:
    if not WINDOWS:
        raise SystemExit(f"{ARGUMENT} only applies on Windows, on Linux build without it")
    wsl = shutil.which("wsl")
    if not wsl:
        raise SystemExit("WSL is not installed")

    env = dict(os.environ)
    env["WSLENV"] = wslenv(env, definitions.REQUIRED)

    sys.stdout.flush()
    return subprocess.run([wsl, "--exec", "sh", "-c", COMMAND, "build.py", *argv], env=env).returncode
