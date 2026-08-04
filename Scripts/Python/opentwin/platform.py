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

_CMAKE_SUBPATH = Path("CommonExtensions") / "Microsoft" / "CMake" / "CMake" / "bin" / "cmake.exe"


def cmake_executable(env: Mapping[str, str]) -> Path:
    """Locate cmake. Windows uses the copy shipped with Visual Studio."""
    if WINDOWS:
        root = env.get("DEVENV_ROOT_2022")
        if not root:
            raise SystemExit("DEVENV_ROOT_2022 is not set")
        cmake = Path(root) / _CMAKE_SUBPATH
        if not cmake.is_file():
            raise SystemExit(f"cmake.exe not found: {cmake}")
        return cmake

    # TODO(linux): no VS-bundled copy, so take cmake from PATH.
    found = shutil.which("cmake", path=env.get("PATH"))
    if not found:
        raise SystemExit("cmake not found on PATH")
    return Path(found)
