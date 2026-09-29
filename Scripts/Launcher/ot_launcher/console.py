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

import subprocess
import sys
from typing import Sequence

from .environment import VARIABLE, Environment, expand, get

VERBOSE = "/V"
PAUSE = {"pause_prefix": 'cmd.exe /S /C "', "pause_suffix": '" ^& pause'}


def verbose(env: Environment, argv: Sequence[str]) -> None:
    if argv and argv[0] == VERBOSE:
        env.update(PAUSE)


def start(env: Environment, line: str) -> None:
    """Runs a START line through cmd, so window titles, quoting and the /V pause work like cmd's START.
    cmd expands the variables that are set; the ones that are not set are left empty."""
    if get(env, "pause_prefix"):
        print(expand(env, line), flush=True)
    command = VARIABLE.sub(lambda match: match.group(0) if get(env, match.group(1)) else "", line)
    # TODO(linux): START and cmd are Windows only.
    shell = get(env, "ComSpec") or "cmd.exe"
    sys.stdout.flush()
    subprocess.run(f'"{shell}" /d /s /c "{command}"', env=dict(env))
