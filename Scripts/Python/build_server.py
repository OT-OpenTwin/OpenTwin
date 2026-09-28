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
import subprocess
import sys
from pathlib import Path
from typing import Callable, Sequence

sys.path.insert(0, str(Path(__file__).resolve().parent))

import build_all
import build_documentation
import clean_all
import create_deployment
import create_frontend_installer
import installers
from ot_dev import cli
from ot_dev.core import paths
from ot_dev.core.output import FAILED_BUILD
from ot_dev.core.process import find_program

WINSCP = ("WinSCP", "WinSCP.com")
UPLOAD_SCRIPT = "BatchBuildAndDeploy.txt"
VERSIONS = "OT_LAST_CONTIBUILD_VERSIONS"
UNCHANGED = 2

REQUIRED = ("OPENTWIN_DEV_ROOT", "OPENTWIN_THIRDPARTY_ROOT", "DEVENV_ROOT_2022", "OPENTWIN_FTP_PASSWORD")
TASK_REQUIRED = REQUIRED + ("OPEN_TWIN_EMAIL_LIST", "OPEN_TWIN_EMAIL_PWD")

SEPARATOR = "=" * 67


def _header(*lines: str) -> None:
    print("\n".join((SEPARATOR, *lines, SEPARATOR)), flush=True)


def _missing(names: Sequence[str]) -> bool:
    for name in names:
        if not os.environ.get(name):
            print(f"Please specify the following environment variables: {name}", flush=True)
            return True
    return False


def _root(name: str, *parts: str) -> Path:
    return Path(os.environ[name]).joinpath(*parts)


def _pull(folder: Path) -> None:
    os.chdir(folder)
    sys.stdout.flush()
    subprocess.run([find_program("git"), "pull"])


def _revision() -> str:
    return subprocess.run([find_program("git"), "rev-parse", "--short", "HEAD"], stdout=subprocess.PIPE, text=True).stdout.strip()


def _call(step: Callable[[Sequence[str]], int], *arguments: str) -> None:
    """A failing step does not stop the job."""
    try:
        step(list(arguments))
    except SystemExit as stop:
        if isinstance(stop.code, str):
            print(stop.code, file=sys.stderr, flush=True)


def _build_failed() -> bool:
    summary = Path.cwd() / paths.SUMMARY
    if not summary.is_file():
        print("The system cannot find the file specified.", file=sys.stderr, flush=True)
        return False
    failed = [line for line in summary.read_text(encoding="utf-8", errors="replace").splitlines()
              if FAILED_BUILD in line]
    for line in failed:
        print(line, flush=True)
    if failed:
        _header("ERROR: Build failed")
    return bool(failed)


def build_and_deploy() -> int:
    if _missing(REQUIRED):
        return 1

    _header("Get the latest version from the repository")
    _pull(_root("OPENTWIN_DEV_ROOT"))
    _pull(_root("OPENTWIN_THIRDPARTY_ROOT"))

    _header("Build the software")
    os.chdir(_root("OPENTWIN_DEV_ROOT", *paths.BUILD_AND_TEST))
    _call(clean_all.main)
    _call(build_all.main, "BOTH", "REBUILD")

    if _build_failed():
        return 1

    _header("Build the documentation")
    os.chdir(_root("OPENTWIN_DEV_ROOT", *paths.BUILD_AND_TEST))
    _call(build_documentation.main)

    _header("Create the deployment")
    os.chdir(_root("OPENTWIN_DEV_ROOT", *paths.BUILD_AND_TEST))
    _call(create_deployment.main)

    _header("Build the frontend installer")
    os.chdir(_root("OPENTWIN_DEV_ROOT", *paths.BUILD_AND_TEST))
    _call(create_frontend_installer.main)

    _header("Build the full installers")
    os.chdir(_root("OPENTWIN_DEV_ROOT", *paths.INSTALLER))
    _call(installers.main, "build")

    _header("Upload the documentation and the nightly installers")
    os.chdir(_root("OPENTWIN_DEV_ROOT", *paths.BUILD_AND_TEST))
    sys.stdout.flush()
    subprocess.run(f'"{_root("OPENTWIN_THIRDPARTY_ROOT", *WINSCP)}" /ini=nul '
                   f'/script="{_root("OPENTWIN_DEV_ROOT", *paths.BUILD_AND_TEST, UPLOAD_SCRIPT)}"')
    return 0


def continuous_build() -> int:
    if _missing(REQUIRED):
        return 1

    _header("Get the latest version from the repository")
    _pull(_root("OPENTWIN_DEV_ROOT"))
    opentwin = _revision()
    _pull(_root("OPENTWIN_THIRDPARTY_ROOT"))
    thirdparty = _revision()

    versions = f"{opentwin}:{thirdparty}"
    if os.environ.get(VERSIONS, "") == versions:
        print('"The software is unchanged. No build necessary"', flush=True)
        return UNCHANGED

    _header("Building Software for following commits:", f"OpenTwin = {opentwin}", f"ThirdParty = {thirdparty}")
    print('""', flush=True)
    subprocess.run(["SETX", VERSIONS, versions], executable=find_program("setx"))

    _header("Build the software")
    os.chdir(_root("OPENTWIN_DEV_ROOT", *paths.BUILD_AND_TEST))
    _call(build_all.main, "BOTH", "BUILD")

    return 1 if _build_failed() else 0


def _task(job: str, log: str, notification: str, result: Callable[[int], str | None]) -> int:
    if _missing(TASK_REQUIRED):
        return 1

    _header("Start the build and deploy batch script")
    os.chdir(_root("OPENTWIN_DEV_ROOT", *paths.BUILD_AND_TEST))
    if not Path(log).exists():
        print(f"Could Not Find {Path.cwd() / log}", file=sys.stderr, flush=True)
    Path(log).unlink(missing_ok=True)

    with open(log, "wb") as out:
        code = subprocess.run([sys.executable, "-u", __file__, job], stdout=out, stderr=subprocess.STDOUT).returncode

    status = result(code)
    if status is None:
        return 0
    sys.stdout.flush()
    return subprocess.run([find_program("powershell.exe"), "-ExecutionPolicy", "Bypass", "-File", notification, status]).returncode


def build_and_deploy_task() -> int:
    return _task("deploy", "buildLog_Nightly.txt", "SendBuildEmailNotification.ps1",
                 lambda code: "SUCCESSFUL" if code == 0 else "FAILED")


def continuous_build_task() -> int:
    return _task("continuous", "buildLog_Continuous.txt", "SendContiBuildEmailNotification.ps1",
                 lambda code: {1: "FAILED", 0: "SUCCESSFUL"}.get(code))


COMMANDS = {
    "deploy": build_and_deploy,
    "deploy-task": build_and_deploy_task,
    "continuous": continuous_build,
    "continuous-task": continuous_build_task,
}


def main(argv: Sequence[str]) -> int:
    if len(argv) != 1 or argv[0] not in COMMANDS:
        raise SystemExit("usage: build_server.py <" + "|".join(COMMANDS) + ">")

    return COMMANDS[argv[0]]()


if __name__ == "__main__":
    sys.exit(cli.run(main, sys.argv[1:]))
