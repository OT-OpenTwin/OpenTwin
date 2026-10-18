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

import importlib
import os
import sys
from pathlib import Path
from typing import Sequence

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))

from ot_dev.core import cli
from ot_dev.core.config import definitions
from ot_dev.core.platform import EDITORS
from ot_dev.core.projects import project_roots

COMPLETE = "--complete"

PROJECT_FIRST = ("build", "clean", "edit", "test")

def scripts() -> list[str]:
    return sorted(path.stem for path in HERE.glob("*.py") if path.stem != Path(__file__).stem)

def project_here() -> tuple[str, Path] | None:
    dev = os.environ.get("OPENTWIN_DEV_ROOT")
    if not dev:
        return None

    keys = {name: key for key, name in project_roots().items()}
    cwd = Path.cwd().resolve()

    for group, folder in definitions.GROUPS:
        for name, project in group.items():
            root = (Path(dev) / folder / project).resolve()
            if cwd == root or root in cwd.parents:
                return keys[name], root

    return None


def command_line(argv: Sequence[str]) -> tuple[str, list[str], Path | None]:
    if not argv or argv[0] not in scripts():
        raise SystemExit("usage: ot <" + "|".join(scripts()) + "> [ARGS...]")

    script, arguments = argv[0], list(argv[1:])
    if script in PROJECT_FIRST and not _names_target(script, arguments):
        here = project_here()
        if here:
            arguments.insert(0, here[0])
            return script, arguments, here[1]

    return script, arguments, None


def _names_target(script: str, arguments: Sequence[str]) -> bool:
    if not arguments:
        return False

    if script == "edit" and Path(arguments[0]).is_absolute():
        return True

    return arguments[0].upper() in project_roots()


def _choices(script: str) -> list[list[str]]:
    configurations, build_types = list(cli.CONFIGURATIONS), list(cli.BUILD_TYPES)
    projects = sorted(project_roots())
    if script == "build_documentation":
        from ot_dev.docs.documentation import MODES
        return [list(MODES)]
    if script in ("helpers", "installers", "build_server"):
        return [list(importlib.import_module(script).COMMANDS)]

    return {
            "build": [projects, configurations, build_types],
            "test": [projects, configurations],
            "clean": [projects],
            "edit": [projects, list(EDITORS)],
            "build_all": [configurations, build_types],
            "build_framework": [configurations, build_types],
            "build_admin_panel": [configurations, build_types],
            "test_all": [configurations],
    }.get(script, [])


def complete(index: int, words: Sequence[str]) -> list[str]:
    current = words[index] if index < len(words) else ""
    if index == 0:
        candidates = scripts()
    else:
        script, given = words[0], list(words[1:index])
        table = _choices(script)
        position = index - 1
        if script in PROJECT_FIRST and project_here() is not None and not _names_target(script, given):
            position += 1

        candidates = table[position] if position < len(table) else []

    return [word for word in candidates if word.upper().startswith(current.upper())]


def main(argv: Sequence[str]) -> int:
    if argv[:1] == [COMPLETE]:
        print("\n".join(complete(int(argv[1]) - 1, argv[2:])))
        return 0

    script, arguments, folder = command_line(argv)
    if folder:
        os.chdir(folder)

    return importlib.import_module(script).main(arguments)

if __name__ == "__main__":
    sys.exit(cli.run(main, sys.argv[1:]))
