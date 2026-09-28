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

"""Starting programs the way cmd does."""

import re
import shutil
import subprocess
import sys
from pathlib import Path
from typing import Mapping, Sequence

from .expansion import expand, merge
from .platform import WINDOWS
from .toolchain import apply_toolchain

# cmd's exit codes when it cannot start a program
NO_PATH = 3
NOT_FOUND = 9009

_ASSIGNMENT = re.compile(r"^[A-Za-z_][A-Za-z0-9_]*=")


def find_program(name: str, path: str | None = None) -> str:
    """Looked up on PATH first, like cmd; CreateProcess would look in System32 before PATH."""
    return shutil.which(name, path=path) or name


def _not_found(program: str) -> int:
    if Path(program).parent != Path(".") and not Path(program).parent.is_dir():
        print("The system cannot find the path specified.", file=sys.stderr, flush=True)
        return NO_PATH
    shown = f'"{program}"' if Path(program).parent != Path(".") else program
    print(f"'{shown}' is not recognized as an internal or external command,\n"
          "operable program or batch file.", file=sys.stderr, flush=True)
    return NOT_FOUND


def run_program(env: Mapping[str, str], command: Sequence[str],
                toolchain: bool = False, detach: bool = False) -> int:
    """Runs a program with the environment. Leading NAME=VALUE entries are set
    first; %VAR% anywhere in the command is expanded with the environment."""
    env = dict(env)
    if toolchain and not env.get("OT_TOOLCHAIN_READY"):
        apply_toolchain(env)
        print("OpenTwin native toolchain was set up successfully.", flush=True)
    command = list(command)
    while command and _ASSIGNMENT.match(command[0]):
        name, _, value = command.pop(0).partition("=")
        merge(env, {name: expand(env, value)})
    if not command:
        raise SystemExit("no program given")

    program, *rest = (expand(env, part) for part in command)
    found = find_program(program, env.get("PATH"))
    if not Path(found).is_file():
        return _not_found(program)

    args, executable = [program, *rest], found
    if WINDOWS and found.lower().endswith((".cmd", ".bat")):
        args, executable = ["cmd", "/c", found, *rest], None
    sys.stdout.flush()

    if detach:
        subprocess.Popen(args, executable=executable, env=env)
        return 0
    return subprocess.run(args, executable=executable, env=env).returncode
