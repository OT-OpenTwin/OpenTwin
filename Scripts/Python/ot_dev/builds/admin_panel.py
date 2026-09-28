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
from pathlib import Path
from typing import Mapping, Sequence

from ..core import paths
from ..core.output import build_result, finish
from ..core.platform import USE_SHELL

ROOT = ("Tools", "AdminPanel")
COMMANDS = [["yarn", "install"], ["yarn", "build"]]


def build_admin_panel(env: Mapping[str, str], configs: Sequence[str],
                      rebuild: bool, logs: str | Path) -> int:
    root = Path(env["OPENTWIN_DEV_ROOT"]).joinpath(*ROOT)
    failed = 0

    print(f"Building Project {root}", flush=True)
    with open(Path(logs) / paths.ADMIN_PANEL_LOG, "w", encoding="utf-8") as out:
        for command in COMMANDS:
            out.write(f"$ {' '.join(command)}\n")
            out.flush()
            code = subprocess.run(command, cwd=root, env=env, stdout=out,
                                  stderr=subprocess.STDOUT, shell=USE_SHELL).returncode
            failed = failed or code
        out.write(build_result(str(root), failed))

    finish(failed)
    return failed
