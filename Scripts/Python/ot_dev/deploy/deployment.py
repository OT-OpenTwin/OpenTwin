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

"""The plans themselves are in Scripts/DeploymentManifest.py."""

import contextlib
import io
import os
import subprocess
import time
from datetime import datetime
from pathlib import Path
from typing import Callable

from ..builds.rebuild import rebuild_all
from ..core import cli, paths
from ..core.config import definitions, manifest
from ..core.environment import build_env, require
from ..core.output import SEPARATOR
from ..core.platform import USE_SHELL
from ..tools.checks import check_failed_builds
from .plan import WILDCARD, Plan, Result, _step, _write
from .progress import Progress
from .services import stop_processes


def _revision(root: Path) -> str:
    result = subprocess.run(["git", "rev-parse", "--short", "HEAD"], cwd=root,
                            capture_output=True, text=True, errors="replace")
    return result.stdout.strip()


def create_build_information(target: Path | None = None) -> int:
    require(definitions.REQUIRED)

    dev = Path(os.environ["OPENTWIN_DEV_ROOT"])
    third = Path(os.environ["OPENTWIN_THIRDPARTY_ROOT"])
    target = Path(target) if target is not None else dev.joinpath(*paths.BUILD_INFO)

    _write(target, (f"Build date/time: {datetime.now():%Y-%m-%d %H:%M:%S} ",
                    f"OpenTwin: {_revision(dev)} ",
                    f"ThirdParty: {_revision(third)} "))

    print(f"Build information written to {target}", flush=True)
    return 0


VERBS = {"copy": "Copying", "tree": "Copying", "mkdir": "Creating", "rmtree": "Removing",
         "remove": "Removing", "remove_glob": "Removing", "write": "Writing",
         "rename": "Renaming", "move": "Moving"}


def _short(path: str) -> str:
    parts = Path(path.rstrip("\\/")).parts
    return "\\".join(parts[-2:])


def _label(step: tuple) -> str:
    if step[0] in ("action", "run"):
        return step[1]
    if step[0] == "tree":
        return f"{VERBS['tree']} {_short(step[2])}"
    if step[0] in ("rmtree", "mkdir"):
        return f"{VERBS[step[0]]} {_short(step[1])}"
    source = Path(step[1])
    if any(character in source.name for character in WILDCARD):
        return f"{VERBS[step[0]]} {source.parent.name}\\{source.name}"
    return f"{VERBS[step[0]]} {source.name}"


def _relative(path: str, base: Path) -> str:
    try:
        return str(Path(path).relative_to(base))
    except ValueError:
        return _short(path)


def _finished(step: tuple, base: Path, result: Result, copied: int, missing: int,
              seconds: float) -> tuple[str, str] | None:
    """Folder steps are the slow ones; each gets a line of its own once it is done."""
    if step[0] == "rmtree":
        return "Removed", f"{_relative(step[1], base):<63.63}{seconds:5.1f}s"
    if step[0] != "tree" or Path(step[1]).is_file():
        return None
    folder = f"{_relative(step[2], base):<50.50}"
    if len(result.missing) > missing:
        return "Skipped", f"{folder}  source missing"
    return "Copied", f"{folder}{result.copied - copied:7} files{seconds:5.1f}s"


def _shutdown(progress: Progress) -> tuple[bool, str]:
    stopped = stop_processes(lambda line: progress.note("", line))
    return True, "OpenTwin: " + (", ".join(stopped) if stopped else "nothing was running")


def _build_information(progress: Progress) -> tuple[bool, str]:
    with contextlib.redirect_stdout(io.StringIO()):
        create_build_information()
    return True, str(Path(*paths.BUILD_INFO))


def _build_all(progress: Progress) -> tuple[bool, str]:
    env = build_env()
    logs = Path(env["OPENTWIN_DEV_ROOT"]).joinpath(*paths.BUILD_AND_TEST)
    code = rebuild_all(env, cli.configurations("BOTH"), cli.build_type("BUILD"))
    check_failed_builds(env)
    return code == 0, f"build all returned {code}, see {logs}"


# name: (title, function, streams its own output)
ACTIONS = {"shutdown": ("Stopped", _shutdown, False),
           "build information": ("Written", _build_information, False),
           "build all": ("Build all", _build_all, True)}


def _execute(plan: Plan, progress: Progress | None = None) -> Result:
    result = Result()
    progress = progress or Progress(0)
    base = Path(plan.env.get("OT_DEPLOYMENT_DIR", ".")).parent
    notes: set[tuple[str, str]] = set()
    for step in plan.steps:
        try:
            if step[0] == "action":
                _action(step[1], progress, result, notes)
            elif step[0] == "run":
                _command(step, progress, result)
            else:
                progress.step(_label(step))
                copied, missing, started = result.copied, len(result.missing), time.monotonic()
                _step(step, result, progress)
                finished = _finished(step, base, result, copied, missing, time.monotonic() - started)
                if finished:
                    progress.note(*finished)
        except OSError as error:
            where = error.filename or step[1]
            result.blocked.append(f"{where} ({error.strerror or error})")
    return result


def _action(name: str, progress: Progress, result: Result, notes: set[tuple[str, str]]) -> None:
    title, function, streams = ACTIONS[name]
    if streams:
        print(f"--- {title} ---", flush=True)
    ok, text = function(progress)
    if not ok:
        result.failed.append(text)
    elif not streams and (title, text) not in notes:
        # CreateDeployment runs UpdateDeploymentLibrariesOnly, which shuts down a second time
        notes.add((title, text))
        progress.note(title, text)


def _command(step: tuple, progress: Progress, result: Result) -> None:
    print(f"--- {step[1]} ---", flush=True)
    code = subprocess.run(step[2], cwd=step[3], shell=USE_SHELL).returncode
    if code:
        result.failed.append(f"{step[1]} returned {code}")


def _deployment_env(env: dict[str, str] | None) -> dict[str, str]:
    env = dict(env if env is not None else build_env())
    dev = Path(env["OPENTWIN_DEV_ROOT"])
    default = str(dev.joinpath(*paths.DEPLOYMENT))
    for name in ("OT_DEPLOYMENT_DIR", "OPENTWIN_DEPLOYMENT_DIR"):
        if not env.get(name):
            env[name] = default
    if not env.get("OPENTWIN_FRONTEND_DEPLOYMENT"):
        env["OPENTWIN_FRONTEND_DEPLOYMENT"] = str(dev.joinpath(*paths.DEPLOYMENT_FRONTEND))
    if not env.get("OPENTWIN_DEBUG_FILES"):
        env["OPENTWIN_DEBUG_FILES"] = str(dev.joinpath(*paths.DEBUG_FILES))
    if not env.get("SYSTEM_32"):
        env["SYSTEM_32"] = str(Path(env.get("SYSTEMROOT", r"C:\Windows")) / "System32")
    return env


def _listing(title: str, entries: list[str], limit: int | None = None) -> None:
    if not entries:
        return
    print(f"{title} ({len(entries)}):", flush=True)
    for entry in entries[:limit]:
        print(f"  {entry}", flush=True)
    if limit is not None and len(entries) > limit:
        print(f"  ... and {len(entries) - limit} more", flush=True)


def _run(title: str, builder: Callable[[Plan], None], env: dict[str, str] | None) -> int:
    started = time.monotonic()
    plan = Plan(_deployment_env(env))
    builder(plan)

    print(title, flush=True)
    print(SEPARATOR, flush=True)
    progress = Progress(sum(1 for step in plan.steps if step[0] not in ("action", "run")))
    try:
        result = _execute(plan, progress)
    finally:
        # also on Ctrl+C, so the console never keeps the scroll region
        progress.close()

    print(SEPARATOR, flush=True)
    _listing("Missing sources", result.missing)
    _listing("Not written, the old file was kept", result.blocked, 5)
    _listing("Failed", result.failed)
    print(f"{result.copied} files copied in {time.monotonic() - started:.0f}s", flush=True)
    if not result:
        print("SUCCESS", flush=True)
        return 0
    counts = ((result.missing, "missing"), (result.blocked, "not written"), (result.failed, "failed"))
    print("FINISHED WITH PROBLEMS: "
          + ", ".join(f"{len(entries)} {what}" for entries, what in counts if entries), flush=True)
    return 1


def update_libraries(env: dict[str, str] | None = None) -> int:
    return _run("Update Deployment Libraries", manifest.update_libraries, env)


def create_deployment(env: dict[str, str] | None = None) -> int:
    return _run("Create Deployment", manifest.create_deployment, env)


def create_frontend_installer(env: dict[str, str] | None = None) -> int:
    return _run("Create Frontend Installer", manifest.create_frontend_installer, env)


def create_debug_files(env: dict[str, str] | None = None) -> int:
    return _run("Create Debug Files", manifest.create_debug_files, env)
