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
from typing import Mapping

from ..core import paths
from ..core.expansion import get
from ..core.process import run_program

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
    seven_zip = Path(get(env, "OPENTWIN_THIRDPARTY_ROOT")).joinpath(*paths.SEVEN_ZIP)
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
    seven_zip = Path(get(env, "OPENTWIN_THIRDPARTY_ROOT")).joinpath(*paths.SEVEN_ZIP)
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
