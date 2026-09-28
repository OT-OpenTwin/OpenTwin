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
import sys
from pathlib import Path
from typing import Callable, Mapping, Sequence

sys.path.insert(0, str(Path(__file__).resolve().parent))

from ot_dev import cli
from ot_dev.tools.archives import compress_all, compress_file, decompress_all, decompress_file
from ot_dev.tools.checks import check_failed_builds
from ot_dev.tools.external import (build_solution, run_cmake_gui, run_cmake_gui_qt, run_devenv, run_python,
                                   run_qtcreator, update_file_headers)
from ot_dev.tools.scheme import register_scheme


def _argument(arguments: Sequence[str], index: int) -> str | None:
    return arguments[index] if len(arguments) > index else None


COMMANDS: dict[str, Callable[[Mapping[str, str], Sequence[str]], int]] = {
    "compress-file": lambda env, a: compress_file(env, _argument(a, 0), _argument(a, 1)),
    "decompress-file": lambda env, a: decompress_file(env, _argument(a, 0), _argument(a, 1)),
    "compress-all": lambda env, a: compress_all(env),
    "decompress-all": lambda env, a: decompress_all(env),
    "update-file-headers": update_file_headers,
    "run-qtcreator": lambda env, a: run_qtcreator(env),
    "cmake-gui-qt": lambda env, a: run_cmake_gui_qt(env, a, qt6_dir=False),
    "cmake-gui-qt6": lambda env, a: run_cmake_gui_qt(env, a, qt6_dir=True),
    "run-cmake": run_cmake_gui,
    "run-devenv": run_devenv,
    "build-solution": lambda env, a: build_solution(env, a[0], a[1:]),
    "run-python": run_python,
    "check-failed-builds": lambda env, a: check_failed_builds(env),
    "register-scheme": lambda env, a: register_scheme(env),
}

# run with the caller's environment instead of build_env()
PLAIN = {"check-failed-builds", "register-scheme"}


def main(argv: Sequence[str]) -> int:
    if not argv or argv[0] not in COMMANDS:
        raise SystemExit("usage: helpers.py <" + "|".join(COMMANDS) + "> [ARGS...]")

    env = dict(os.environ) if argv[0] in PLAIN else cli.environment()
    return COMMANDS[argv[0]](env, argv[1:])


if __name__ == "__main__":
    sys.exit(cli.run(main, sys.argv[1:]))
