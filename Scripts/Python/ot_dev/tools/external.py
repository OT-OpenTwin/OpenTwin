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

from pathlib import Path
from typing import Mapping, Sequence

from ..core import paths
from ..core.expansion import get
from ..core.platform import EXE
from ..core.process import run_program
from ..core.toolchain import apply_toolchain

# TODO(linux): Qt Creator, Visual Studio and the legacy Python are Windows installs.
QT_CREATOR = ("Qt", "Tools", "QtCreator", "bin", "qtcreator" + EXE)
DEVENV = "devenv" + EXE
FILE_HEADER_UPDATER = paths.RELEASE_OUTPUT + ("FileHeaderUpdater" + EXE,)
FILE_HEADER_LOG = "FHU_Log.txt"
FILE_HEADER_CONFIG = "OT_FHU_Config.json"

QT_DIRS = {"Qt6Core_DIR": "Qt6Core", "Qt6Gui_DIR": "Qt6Gui", "Qt6Widgets_DIR": "Qt6Widgets"}

# the .bat wrappers pass at most %1 to %9
BATCH_ARGUMENTS = 9


def _qt_env(env: Mapping[str, str], qt6_dir: bool) -> list[str]:
    cmake = Path(get(env, "QDIR")) / "lib" / "cmake"
    assignments = [f"Qt6_DIR={cmake}"] if qt6_dir else []
    return assignments + [f"{name}={cmake / folder}" for name, folder in QT_DIRS.items()]


def update_file_headers(env: Mapping[str, str], arguments: Sequence[str]) -> int:
    root = Path(get(env, "OT_FILEHEADERUPDATER_ROOT"))
    return run_program(env, [str(root.joinpath(*FILE_HEADER_UPDATER)), "--out", str(root / FILE_HEADER_LOG),
                             "--config", str(root / FILE_HEADER_CONFIG), *arguments[:BATCH_ARGUMENTS]])


def run_qtcreator(env: Mapping[str, str]) -> int:
    creator = Path(get(env, "OPENTWIN_THIRDPARTY_ROOT")).joinpath(*QT_CREATOR)
    print("Setup Qt6 enviroment", flush=True)
    print("call qtcreator", flush=True)
    print(creator, flush=True)
    return run_program(env, [*_qt_env(env, qt6_dir=False), str(creator)])


def run_cmake_gui_qt(env: Mapping[str, str], arguments: Sequence[str], qt6_dir: bool) -> int:
    print("Setup Qt6 enviroment", flush=True)
    print("call cmake-gui", flush=True)
    return run_program(env, [*_qt_env(env, qt6_dir), "cmake-gui", *arguments])


def run_cmake_gui(env: Mapping[str, str], arguments: Sequence[str]) -> int:
    env = dict(env)
    if not get(env, "OT_TOOLCHAIN_READY"):
        apply_toolchain(env)
        print("OpenTwin native toolchain was set up successfully.", flush=True)
    print("Launching development enviroment", flush=True)
    return run_program(env, ["cmake-gui", *arguments[:1]])


def run_devenv(env: Mapping[str, str], arguments: Sequence[str]) -> int:
    print("Launching development enviroment", flush=True)
    devenv = Path(get(env, "DEVENV_ROOT_2022")) / DEVENV
    return run_program(env, [str(devenv), *arguments[:1]], detach=True)


def build_solution(env: Mapping[str, str], solution: str, arguments: Sequence[str]) -> int:
    configuration = arguments[0] if arguments else ""
    kind = arguments[1] if len(arguments) > 1 else ""
    debug = configuration != "RELEASE"
    release = configuration != "DEBUG"
    switch, name = ("/Build", "BUILD") if kind == "BUILD" else ("/Rebuild", "REBUILD")

    print("Building Project", flush=True)
    devenv = Path(get(env, "DEVENV_ROOT_2022")) / DEVENV
    code = 0
    for enabled, label, platform, log in ((debug, "DEBUG", "Debug|x64", "buildLog_Debug.txt"),
                                          (release, "RELEASE", "Release|x64", "buildLog_Release.txt")):
        if enabled:
            print(f"{name} {label}", flush=True)
            code = run_program(env, [str(devenv), solution, switch, platform, "/Out", log])
    return code


def run_python(env: Mapping[str, str], arguments: Sequence[str]) -> int:
    return run_program(env, ["PYTHONPATH=%OT_PYTHONPATH_LEGACY%", "PATH=%OT_PYTHONPATH_LEGACY%;%PATH%",
                             "python", *arguments[:BATCH_ARGUMENTS]])
