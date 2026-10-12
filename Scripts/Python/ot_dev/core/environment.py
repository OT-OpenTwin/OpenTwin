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
from pathlib import Path
from typing import Iterable, Mapping

from .config import definitions
from .expansion import expand, load_module
from .platform import REQUIRED as PLATFORM_REQUIRED, WINDOWS, native_path

# WSL has its own PATH
WSLENV_EXCLUDE = {"PATH"}
WINDOWS_PATH = re.compile(r"^[A-Za-z]:[\\/][^;]*$")


def require(names: Iterable[str]) -> None:
    missing = [name for name in names if not os.environ.get(name)]
    if missing:
        raise SystemExit("Please specify the following environment variables: " + ", ".join(missing))


def check_required() -> None:
    require(list(definitions.REQUIRED) + PLATFORM_REQUIRED)


def _thirdparty(env: dict[str, str], third: Path) -> None:
    load_module("ot_thirdparty_env", third / definitions.THIRDPARTY_MODULE).apply(env)


def _projects(env: dict[str, str], dev: Path) -> None:
    env.update({name: native_path(value) for name, value in definitions.RELATIVE.items()})
    for group, folder in definitions.GROUPS:
        for name, project in group.items():
            env[name] = str(dev / folder / project)


def _dev_paths(env: dict[str, str], dev: Path) -> None:
    for name, relative in definitions.DEV_PATHS.items():
        env[name] = str(dev / native_path(relative))


def _certificates(env: dict[str, str], dev: Path) -> None:
    root = Path(env.get(definitions.CERTIFICATES_OVERRIDE) or dev / native_path(definitions.CERTIFICATES_ROOT))
    for name, filename in definitions.CERTIFICATES.items():
        env[name] = str(root / filename)


def _composites(env: dict[str, str]) -> None:
    for name, template in definitions.COMPOSITES.items():
        env[name] = native_path(expand(env, template))


def _service_args(env: dict[str, str], dev: Path) -> None:
    module = load_module("ot_service_args", dev.joinpath(*definitions.SERVICE_ARGS_MODULE))
    for name, template in module.DEFAULTS.items():
        if name not in env:
            env[name] = expand(env, template)


def _path(env: dict[str, str]) -> None:
    for entry in definitions.PATH_PREPEND:
        env["PATH"] = expand(env, f"{native_path(entry)}{os.pathsep}%PATH%")


def _wslenv(env: dict[str, str], before: Mapping[str, str]) -> None:
    # Visual Studio building in WSL starts wsl.exe from this environment: WSLENV makes
    # wsl.exe pass the variables set here on to Linux, /p converts a path to /mnt/<drive>/...
    names = list(definitions.REQUIRED)
    names += [name for name, value in env.items()
              if before.get(name) != value and name not in names and name not in WSLENV_EXCLUDE]
    entries = [f"{name}/p" if WINDOWS_PATH.match(env[name]) else name for name in names]
    env["WSLENV"] = ":".join(filter(None, [before.get("WSLENV"), *entries]))


def build_env(base: str | Path | None = None) -> dict[str, str]:
    check_required()
    env = os.environ.copy()
    before = dict(env)
    dev = Path(base or env["OPENTWIN_DEV_ROOT"])
    third = Path(env["OPENTWIN_THIRDPARTY_ROOT"])

    _thirdparty(env, third)
    _projects(env, dev)
    _dev_paths(env, dev)
    _certificates(env, dev)
    _composites(env)
    _service_args(env, dev)
    _path(env)

    env[definitions.READY_FLAG] = "1"
    if WINDOWS:
        _wslenv(env, before)
    print("OpenTwin Developer environment was set up successfully.")
    return env
