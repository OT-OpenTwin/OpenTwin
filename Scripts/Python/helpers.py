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

import sys
from pathlib import Path
from typing import Callable, Mapping, Sequence

sys.path.insert(0, str(Path(__file__).resolve().parent))

from ot_dev import apply_toolchain, cli, run_program
from ot_dev.expansion import get
from ot_dev.platform import WINDOWS

SUFFIX = ".exe" if WINDOWS else ""

# TODO(linux): 7-Zip, Qt Creator, Visual Studio and the legacy Python are Windows installs.
SEVEN_ZIP = ("7-Zip", "Win64", "7z" + SUFFIX)
QT_CREATOR = ("Qt", "Tools", "QtCreator", "bin", "qtcreator" + SUFFIX)
DEVENV = "devenv" + SUFFIX
FILE_HEADER_UPDATER = ("build", "windows-release", "Release", "FileHeaderUpdater" + SUFFIX)
FILE_HEADER_LOG = "FHU_Log.txt"
FILE_HEADER_CONFIG = "OT_FHU_Config.json"

QT_DIRS = {"Qt6Core_DIR": "Qt6Core", "Qt6Gui_DIR": "Qt6Gui", "Qt6Widgets_DIR": "Qt6Widgets"}

# ThirdParty files too large for git, relative to OPENTWIN_THIRDPARTY_ROOT
QT_BIN = r"Qt\6.6.1\msvc2019_64\bin"
QT_LIB = r"Qt\6.6.1\msvc2019_64\lib"
DESIGN_STUDIO = r"Qt\Tools\QtDesignStudio\qt6_design_studio_reduced_version\bin"
LARGE_FILES = (
    (QT_BIN, "Qt6Guid.pdb", QT_BIN),
    (QT_BIN, "Qt6Pdfd.pdb", QT_BIN),
    (QT_BIN, "Qt6Qmld.pdb", QT_BIN),
    (QT_BIN, "Qt6Quickd.pdb", QT_BIN),
    (QT_BIN, "Qt63DRenderd.pdb", QT_BIN),
    (QT_LIB, "Qt6QmlDomd.lib", QT_LIB),
    (QT_LIB, "Qt6BundledPhysXd.lib", QT_LIB),
    (QT_LIB, "Qt6QmlLSd.lib", QT_LIB),
    (QT_LIB, "Qt6BundledPhysX.lib", QT_LIB),
    (QT_LIB, "Qt6QmlDom.lib", QT_LIB),
    (QT_LIB, "Qt6QmlLS.lib", QT_LIB),
    (QT_LIB, "Qt6BundledResonanceAudiod.lib", QT_LIB),
    (DESIGN_STUDIO, "Qt6WebEngineCore.dll", QT_LIB),
    (DESIGN_STUDIO, "Qt6WebEngineCored.dll", QT_LIB),
)

BATCH_ARGUMENTS = 9


def _qt_env(env: Mapping[str, str], qt6_dir: bool) -> list[str]:
    cmake = Path(get(env, "QDIR")) / "lib" / "cmake"
    assignments = [f"Qt6_DIR={cmake}"] if qt6_dir else []
    return assignments + [f"{name}={cmake / folder}" for name, folder in QT_DIRS.items()]


def compress_file(env: Mapping[str, str], source: str | None, archive: str | None) -> int:
    if not source:
        print("Please specify a source file when running this script", flush=True)
        return 0
    if not archive:
        print("Please specify a destination file when running this script", flush=True)
        return 0
    if not Path(source).exists():
        print(f'Source file does not exist ""{source}""', flush=True)
        return 0
    if Path(archive).exists():
        print(f'Compressed file already exists ""{archive}""', flush=True)
        return 0
    seven_zip = Path(get(env, "OPENTWIN_THIRDPARTY_ROOT")).joinpath(*SEVEN_ZIP)
    return run_program(env, [str(seven_zip), "a", archive, source])


def decompress_file(env: Mapping[str, str], archive: str | None, folder: str | None) -> int:
    if not archive:
        print("Please specify a file when running this script", flush=True)
        return 0
    if not folder:
        print("Please specify a output directory for the file", flush=True)
        return 0
    if not Path(archive).exists():
        print(f'Source file does not exist ""{archive}""', flush=True)
        return 0
    if not Path(folder).exists():
        print('Destination folder does not exists ""', flush=True)
        return 0
    seven_zip = Path(get(env, "OPENTWIN_THIRDPARTY_ROOT")).joinpath(*SEVEN_ZIP)
    return run_program(env, [str(seven_zip), "x", f"-o{folder}", archive])


def compress_all(env: Mapping[str, str]) -> int:
    third = Path(get(env, "OPENTWIN_THIRDPARTY_ROOT"))
    for folder, name, _ in LARGE_FILES:
        compress_file(env, str(third / folder / name), str(third / folder / Path(name).with_suffix(".7z")))
    return 0


def decompress_all(env: Mapping[str, str]) -> int:
    third = Path(get(env, "OPENTWIN_THIRDPARTY_ROOT"))
    for folder, name, target in LARGE_FILES:
        decompress_file(env, str(third / folder / Path(name).with_suffix(".7z")), str(third / target))
    return 0


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
}


def main(argv: Sequence[str]) -> int:
    if not argv or argv[0] not in COMMANDS:
        raise SystemExit("usage: helpers.py <" + "|".join(COMMANDS) + "> [ARGS...]")

    return COMMANDS[argv[0]](cli.environment(), argv[1:])


if __name__ == "__main__":
    sys.exit(cli.run(main, sys.argv[1:]))
