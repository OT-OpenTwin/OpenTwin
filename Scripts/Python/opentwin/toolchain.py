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
import re
import subprocess
from pathlib import Path
from typing import Any, Mapping

from .expansion import expand, merge, unique

TOOLCHAIN: dict[str, dict[str, Any]] = {
    "nt": {
        "install_root": r"%DEVENV_ROOT_2022%\..\..",
        "script": r"VC\Auxiliary\Build\vcvars64.bat",
        "verify": ["INCLUDE", "LIB", "LIBPATH", "PATH"],
        "reset": ["VSCMD_VER", "INCLUDE", "LIB", "LIBPATH"],
        "ready": "OT_TOOLCHAIN_READY",
    },
}

_ENV_NAME = re.compile(r"^[A-Za-z_][A-Za-z0-9_()]*$")


def _script(env: Mapping[str, str], spec: Mapping[str, Any]) -> Path:
    script = (Path(expand(env, spec["install_root"])) / spec["script"]).resolve()
    if not script.is_file():
        raise SystemExit(f"Native toolchain not found: {script}")
    return script


def _capture(env: Mapping[str, str], script: Path) -> dict[str, str]:
    result = subprocess.run(["cmd", "/c", str(script), "&&", "set"], env=env,
                            capture_output=True, text=True, errors="replace")
    if result.returncode != 0:
        raise SystemExit(f"Toolchain setup failed ({result.returncode}): {script}\n{result.stdout.strip()}")

    captured: dict[str, str] = {}
    for line in result.stdout.splitlines():
        name, separator, value = line.partition("=")
        if separator and _ENV_NAME.match(name):
            captured[name] = value
    return captured


def apply_toolchain(env: dict[str, str]) -> dict[str, str]:
    spec = TOOLCHAIN.get(os.name)
    if not spec or env.get(spec["ready"]):
        return env

    script = _script(env, spec)
    reset = {name.lower() for name in spec["reset"]}
    captured = _capture({k: v for k, v in env.items() if k.lower() not in reset}, script)

    missing = [name for name in spec["verify"] if not captured.get(name)]
    if missing:
        raise SystemExit(f"Toolchain setup incomplete: {script} did not provide " + ", ".join(missing))

    merge(env, captured)
    env["PATH"] = os.pathsep.join(unique(env["PATH"].split(os.pathsep)))
    env[spec["ready"]] = "1"
    return env
