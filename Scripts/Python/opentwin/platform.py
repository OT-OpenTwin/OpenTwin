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

"""Platform specifics for the OpenTwin build scripts.

TODO(linux): path separators are not handled here on purpose.
OT_ALL_DLLD is read both as a CMake list (";" everywhere)
and as a PATH (":" on Linux), so it needs splitting, not a separator swap.
"""

import os
import shutil
from pathlib import Path
from typing import Mapping

WINDOWS = os.name == "nt"

SYSTEM = "windows" if WINDOWS else "linux"

# Applied to build subprocesses.
ENV_VARS: dict[str, str] = {"VSLANG": "1033"} if WINDOWS else {}

# Added to the cross-platform names in SetupEnvironment.REQUIRED.
REQUIRED: list[str] = ["DEVENV_ROOT_2022"] if WINDOWS else []

# yarn/cargo are .cmd shims on Windows; on POSIX a list argv must not use a shell.
USE_SHELL = WINDOWS

DEFAULT_EDITOR = "VS" if WINDOWS else "CODE"

EDITORS: dict[str, tuple[str | None, str]] = {
    "VS": ("DEVENV_ROOT_2022", "devenv.exe"),
    "CODE": (None, "code"),
    "NVIM": (None, "nvim"),
}

_CMAKE_BIN = Path("CommonExtensions") / "Microsoft" / "CMake" / "CMake" / "bin"


def _devenv_tool(env: Mapping[str, str], name: str) -> Path:
    root = env.get("DEVENV_ROOT_2022")
    if not root:
        raise SystemExit("DEVENV_ROOT_2022 is not set")
    tool = Path(root) / _CMAKE_BIN / name
    if not tool.is_file():
        raise SystemExit(f"{name} not found: {tool}")
    return tool


def cmake_executable(env: Mapping[str, str]) -> Path:
    """Locate cmake. Windows uses the copy shipped with Visual Studio."""
    if WINDOWS:
        return _devenv_tool(env, "cmake.exe")

    # TODO(linux): no VS-bundled copy, so take cmake from PATH.
    found = shutil.which("cmake", path=env.get("PATH"))
    if not found:
        raise SystemExit("cmake not found on PATH")
    return Path(found)


def ctest_executable(env: Mapping[str, str]) -> Path:
    """Locate ctest, from the same bundle as cmake_executable. A bare ctest on
    PATH can resolve to an unrelated standalone CMake install."""
    if WINDOWS:
        return _devenv_tool(env, "ctest.exe")

    # TODO(linux): no VS-bundled copy, so take ctest from PATH.
    found = shutil.which("ctest", path=env.get("PATH"))
    if not found:
        raise SystemExit("ctest not found on PATH")
    return Path(found)
