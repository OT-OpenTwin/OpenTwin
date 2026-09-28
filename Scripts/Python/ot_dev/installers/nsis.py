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

"""The NSIS installers: end user and developer installer, and the MongoDB upgrader."""

import shutil
import subprocess
import sys
from pathlib import Path
from typing import Mapping

from ..builds.project import build_project
from ..core import paths
from ..core.projects import resolve_root
from ..deploy.services import shutdown_all
from .registry import read_value

# TODO(linux): NSIS installers, the registry and 7-Zip are Windows only.
NSIS_KEY = ("HKEY_LOCAL_MACHINE", r"SOFTWARE\WOW6432Node\NSIS", "")
SEVEN_ZIP_KEY = ("HKEY_CURRENT_USER", r"SOFTWARE\7-Zip", "Path")

# relative to OPENTWIN_THIRDPARTY_ROOT
UNZIP = ("Installer_Tools",)
THIRDPARTY_ZIP = ("Installer_Tools", "ThirdParty.zip.001")
NSIS_LOG_ZIP = ("Installer_Tools", "ThirdParty", "dev", "nsis-3.10-log.zip")
SHARED = ("Installer_Tools", "ThirdParty", "shared")
MONGODB_MSI = "mongodb-windows-x86_64-7.0.14-signed.msi"

# relative to OPENTWIN_DEV_ROOT
IMAGES = "InstallationImages"
HELPER = paths.INSTALLER + ("helper",)
BUILD_INFO = "BuildInfo.txt"
ENDUSER_NSI = paths.INSTALLER + ("nsis", "install_opentwin_endUser.nsi")
DEVELOPER_NSI = paths.INSTALLER + ("nsis", "install_opentwin_Developer.nsi")
UPGRADER_NSI = ("Tools", "MongoDBUpgrader", "Upgrader_NSIS", "MongoDBUpgrader_Standalone.nsi")
UPGRADER_DEPLOYMENT = ("Tools", "MongoDBUpgrader", "Upgrader_Deployment")
UPGRADER = "MONGODBUPGRADEMANAGER"
UPGRADER_EXE = "MongoDBUpgradeManager.exe"

CONFIGURATION_TOOLS = ("SetPermissions", "ConfigMongoDBNoAuth", "ConfigMongoDBWithAuth")

BOOST_DLL = (r"boost\boost_1_86_0\lib64-msvc-14.3", "boost_filesystem-vc143-mt-x64-1_86.dll")
MONGO_BINS = (r"MongoDb\mongo-cxx-driver-r3.10.0\x64\Release\bin", r"MongoDb\mongo-c-driver-1.27.3\x64\Release\bin")
ZLIB_DLL = (r"zlib\zlib-1.2.11\x64\Release\bin", "zlib.dll")

LINE = "-" * 61
FAIL_LINE = "-" * 45
BOTH = ["debug", "release"]


def _copy(source: Path, target: Path) -> None:
    """A folder copies its files, a folder target keeps the file name. Prints what COPY prints."""
    if source.is_dir():
        files = sorted((entry for entry in source.iterdir() if entry.is_file()), key=lambda f: f.name.upper())
        for file in files:
            print(file, flush=True)
            shutil.copy2(file, target / file.name if target.is_dir() else target)
        print(f"{len(files):>9} file(s) copied.", flush=True)
    elif source.is_file():
        shutil.copy2(source, target / source.name if target.is_dir() else target)
        print("        1 file(s) copied.", flush=True)
    elif source.parent.is_dir():
        print("The system cannot find the file specified.", file=sys.stderr, flush=True)
    else:
        print("The system cannot find the path specified.", file=sys.stderr, flush=True)


# 7z, xcopy and makensis get command line strings, the odd quoting is deliberate.
def _run(command: str, env: Mapping[str, str], cwd: Path | None = None) -> int:
    sys.stdout.flush()
    return subprocess.run(command, env=env, cwd=cwd).returncode


def _makensis(nsis: str, folder: str, name: str, script: Path, env: Mapping[str, str],
              cwd: Path | None = None) -> int:
    return _run(f'"{nsis}\\makensis.exe" /V3 "/XOutFile "{folder}\\{name}"" "{script}"', env, cwd)


def _verify_nsis(nsis: str) -> bool:
    if not nsis:
        print("NSIS Installation not found", flush=True)
        return False
    print(f"NSIS Installation verified in '{nsis}'...", flush=True)
    print(LINE)
    print("Script compilation will take a few minutes, please be patient")
    print("Ready to compile")
    print(LINE)
    print("+++ COMPILE TIME +++", flush=True)
    return True


def _failed() -> int:
    print(FAIL_LINE)
    print("ERROR: The script has enountered an issue. Please try again.", flush=True)
    return 1


def _succeeded() -> int:
    print(FAIL_LINE)
    print("Script compilation has finished successfully. Exiting...", flush=True)
    return 0


def _remove(folder: Path) -> None:
    if not folder.exists():
        print("The system cannot find the file specified.", file=sys.stderr, flush=True)
    shutil.rmtree(folder, ignore_errors=True)


def _fresh(folder: Path) -> None:
    _remove(folder)
    folder.mkdir(parents=True, exist_ok=True)


def create_installer_build_information(env: Mapping[str, str]) -> int:
    dev = Path(env["OPENTWIN_DEV_ROOT"])
    info = dev.joinpath(*HELPER) / BUILD_INFO
    if not info.parent.is_dir():
        print("The system cannot find the path specified.", file=sys.stderr, flush=True)
        return 1

    with open(info, "ab") as out:
        out.write(b"This installer was created with the following revisions: \r\n")
        for label, repository in ((b"OpenTwin Repo: ", dev), (b"Third Party Repo: ", Path(env["OPENTWIN_THIRDPARTY_ROOT"]))):
            out.write(label + b"\r\n")
            out.flush()
            subprocess.run(["git", "rev-parse", "--short", "HEAD"], cwd=repository, stdout=out)
    return 0


def build_installers(env: Mapping[str, str]) -> int:
    dev = Path(env["OPENTWIN_DEV_ROOT"])
    third = Path(env["OPENTWIN_THIRDPARTY_ROOT"])

    shutdown_all()

    images = env.get("OT_INSTALLIMAGES_DIR") or f'"{dev / IMAGES}"'
    _fresh(Path(images.strip('"')))

    nsis = read_value(*NSIS_KEY)
    seven_zip = read_value(*SEVEN_ZIP_KEY)
    if not seven_zip:
        print("ERROR: 7Zip is not installed on your system", flush=True)
        return _failed()
    print(f"7Zip Installation verified in '{seven_zip}'...", flush=True)
    if not _verify_nsis(nsis):
        return _failed()

    seven_zip_exe = f"{seven_zip}\\7z.exe"
    print("Extracting Third Party Toolchain using 7-Zip...", flush=True)
    _remove(third.joinpath(*UNZIP, "ThirdParty"))
    _run(f'"{seven_zip_exe}" x "{third.joinpath(*THIRDPARTY_ZIP)}" -o"{third.joinpath(*UNZIP)}" -y', env)

    print(f"NSIS found: {nsis}", flush=True)
    print("Extracting special build of NSIS including logging...", flush=True)
    _run(f'"{seven_zip_exe}" x ""{third.joinpath(*NSIS_LOG_ZIP)}"" -o"{nsis}" -y -aoa', env)

    print("Copying Installation helpers...", flush=True)
    helper = dev.joinpath(*HELPER)
    _fresh(helper)
    create_installer_build_information(env)

    configuration = helper / "Configuration"
    configuration.mkdir()
    for tool in CONFIGURATION_TOOLS:
        _copy(dev.joinpath("Tools", tool, *paths.RELEASE_OUTPUT, tool + ".exe"), configuration)
    _copy(dev.joinpath("Libraries", "OTSystem", *paths.RELEASE_OUTPUT, "OTSystem.dll"), configuration)

    upgrader = Path(resolve_root(env, UPGRADER))
    upgrader_exe = helper / "Upgrader_Exe"
    upgrader_exe.mkdir()
    build_project(env, str(upgrader), BOTH, True, helper)
    _copy(upgrader.joinpath(*paths.RELEASE_OUTPUT, UPGRADER_EXE), upgrader_exe / UPGRADER_EXE)

    _copy(third / BOOST_DLL[0] / BOOST_DLL[1], upgrader_exe / BOOST_DLL[1])
    for folder in MONGO_BINS:
        _copy(third / folder, upgrader_exe)
    _copy(third / ZLIB_DLL[0] / ZLIB_DLL[1], upgrader_exe / ZLIB_DLL[1])
    for library in ("OTSystem", "OTCore"):
        _copy(dev.joinpath("Libraries", library, *paths.RELEASE_OUTPUT, library + ".dll"), upgrader_exe / (library + ".dll"))

    print("COMPILING OPENTWIN INSTALLATION SCRIPTS", flush=True)
    _makensis(nsis, images, "Install_OpenTwin.exe", dev.joinpath(*ENDUSER_NSI), env, helper)
    _makensis(nsis, images, "Install_OpenTwin_Development.exe", dev.joinpath(*DEVELOPER_NSI), env, helper)
    return _succeeded()


def build_upgrader(env: Mapping[str, str]) -> int:
    dev = Path(env["OPENTWIN_DEV_ROOT"])
    third = Path(env["OPENTWIN_THIRDPARTY_ROOT"])

    deployment = dev.joinpath(*UPGRADER_DEPLOYMENT)
    _fresh(deployment)
    nsis = read_value(*NSIS_KEY)
    for folder in ("MongoDB_Server", "MongoDB_Installer", "Upgrader_Exe"):
        (deployment / folder).mkdir()
    if not _verify_nsis(nsis):
        return _failed()

    print("Extracting Third Party Toolchain using 7-Zip...", flush=True)
    _run(f'"{third.joinpath(*paths.SEVEN_ZIP)}" x "{third.joinpath(*THIRDPARTY_ZIP)}" -o"{third.joinpath(*UNZIP)}" -y', env)

    upgrader = Path(resolve_root(env, UPGRADER))
    upgrader_exe = deployment / "Upgrader_Exe"
    build_project(env, str(upgrader), BOTH, True)
    _copy(upgrader.joinpath(*paths.RELEASE_OUTPUT, UPGRADER_EXE), upgrader_exe / UPGRADER_EXE)

    _copy(third / BOOST_DLL[0] / BOOST_DLL[1], upgrader_exe / BOOST_DLL[1])
    for folder in MONGO_BINS:
        _copy(third / folder, upgrader_exe)
    for root, library in (("OT_SYSTEM_ROOT", "OTSystem.dll"), ("OT_CORE_ROOT", "OTCore.dll")):
        _copy(Path(env[root]).joinpath(*paths.RELEASE_OUTPUT, library), upgrader_exe)

    shared = third.joinpath(*SHARED)
    _copy(shared / "MongoDB_Installer" / MONGODB_MSI, deployment / "MongoDB_Installer" / MONGODB_MSI)
    _run(f'xcopy /e "{shared / "MongoDB_Server"}\\" "{deployment}"\\MongoDB_Server\\', env)

    print("COMPILING MONGODB UPGRADER SCRIPT", flush=True)
    _makensis(nsis, f'"{deployment}"', "UpgradeMongoDB.exe", dev.joinpath(*UPGRADER_NSI), env)
    return _succeeded()
