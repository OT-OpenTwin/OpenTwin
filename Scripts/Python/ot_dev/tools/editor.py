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

import shutil
import subprocess
from pathlib import Path
from typing import Mapping

from ..core.expansion import get
from ..core.platform import DEFAULT_EDITOR, EDITORS, WINDOWS

EDITOR_VARIABLE = "OT_DEFAULT_EDITOR"


def _rooted(env: Mapping[str, str], root: str, executable: str, target: str) -> int:
    command = Path(env[root]) / executable
    if not command.is_file():
        raise SystemExit(f"{executable} not found: {command}")
    subprocess.Popen([str(command), target], env=env)
    return 0


def _on_path(env: Mapping[str, str], executable: str, target: str) -> int:
    command = shutil.which(executable, path=env.get("PATH"))
    if not command:
        raise SystemExit(f"{executable} not found on PATH")

    args = [command, target]
    if WINDOWS and command.lower().endswith((".cmd", ".bat")):
        args = ["cmd", "/c", *args]
    return subprocess.run(args, env=env).returncode


def launch_editor(env: Mapping[str, str], target: str, editor: str | None = None) -> int:
    chosen = editor or get(env, EDITOR_VARIABLE).strip() or DEFAULT_EDITOR
    key = chosen.upper()
    if key not in EDITORS:
        source = "" if editor else f" in {EDITOR_VARIABLE}"
        raise SystemExit(f"Unknown editor '{chosen}'{source}. Known: " + ", ".join(sorted(EDITORS)))
    if not Path(target).exists():
        raise SystemExit(f"path does not exist: {target}")

    root, executable = EDITORS[key]
    print(f"Launching {key}", flush=True)
    if root:
        return _rooted(env, root, executable, target)
    return _on_path(env, executable, target)

