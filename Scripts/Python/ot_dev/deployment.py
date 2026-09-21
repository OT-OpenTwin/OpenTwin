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

import ctypes
import fnmatch
import os
import shutil
import subprocess
import sys
import time
from datetime import datetime
from pathlib import Path
from typing import Callable

from .actions import SEPARATOR
from .config import definitions, manifest, order
from .environment import build_env
from .expansion import expand
from .platform import USE_SHELL, WINDOWS
from .services import shutdown_all

BUILD_INFO = ("Deployment", "BuildInfo.txt")

WILDCARD = ("*", "?")

ANY = ("*.*", "*")


def _revision(root: Path) -> str:
    result = subprocess.run(["git", "rev-parse", "--short", "HEAD"], cwd=root,
                            capture_output=True, text=True, errors="replace")
    return result.stdout.strip()


def _write(target: Path, lines) -> None:
    with open(target, "w", newline="") as handle:
        handle.writelines(f"{line}\r\n" for line in lines)


def create_build_information(target: Path | None = None) -> int:
    missing = [name for name in definitions.REQUIRED if not os.environ.get(name)]
    if missing:
        raise SystemExit("Please specify the following environment variables: " + ", ".join(missing))

    dev = Path(os.environ["OPENTWIN_DEV_ROOT"])
    third = Path(os.environ["OPENTWIN_THIRDPARTY_ROOT"])
    target = Path(target) if target is not None else dev.joinpath(*BUILD_INFO)

    _write(target, (f"Build date/time: {datetime.now():%Y-%m-%d %H:%M:%S} ",
                    f"OpenTwin: {_revision(dev)} ",
                    f"ThirdParty: {_revision(third)} "))

    print(f"Build information written to {target}", flush=True)
    return 0


BAR = 28

REFRESH = 0.05


class Progress:
    """Single line bar; falls back to plain lines when output is redirected."""

    def __init__(self, total: int) -> None:
        self.total = max(total, 1)
        self.done = 0
        self.live = sys.stdout.isatty()
        self._last = 0.0

    def step(self, label: str) -> None:
        self.done += 1
        self._draw(label)

    def detail(self, label: str) -> None:
        # throttled by time so naming every file costs nothing
        if not self.live:
            return
        now = time.monotonic()
        if now - self._last < REFRESH:
            return
        self._last = now
        self._draw(label)

    def _draw(self, label: str) -> None:
        filled = BAR * self.done // self.total
        bar = "#" * filled + "." * (BAR - filled)
        line = f"  [{bar}] {self.done:4}/{self.total}  {label:<46.46}"
        prefix = "\r" if self.live else ""
        ending = "" if self.live else "\n"
        print(prefix + line, end=ending, flush=True)

    def pause(self) -> None:
        if self.live:
            print(flush=True)

    def close(self) -> None:
        if self.live:
            print(flush=True)


def _label(step: tuple) -> str:
    if step[0] in ("action", "run"):
        return step[1]
    source = Path(step[1])
    if any(character in source.name for character in WILDCARD):
        return f"{step[0]} {source.parent.name}/{source.name}"
    return f"{step[0]} {source.name}"


def _build_all() -> int:
    """CreateDebugFiles.bat calls BuildAll.bat, which is RebuildAll.bat BOTH BUILD."""
    import build_all as script

    from . import cli

    env = build_env()
    logs = Path(env["OPENTWIN_DEV_ROOT"]).joinpath(*script.LOG_DIR)
    return script.build_all(env, order.BUILD_ORDER, order.BUILD_OVERRIDES, script.SPECIAL,
                            cli.configurations("BOTH"), cli.build_type("BUILD"),
                            logs, script.SUMMARY)


ACTIONS = {"shutdown": shutdown_all,
           "build information": create_build_information,
           "build all": _build_all}


class Plan:
    """Ordered deployment steps, inspectable before anything runs."""

    def __init__(self, env: dict[str, str]) -> None:
        self.env = env
        self.steps: list[tuple] = []

    def _at(self, value: str) -> str:
        return expand(self.env, value)

    def copy(self, source: str, target: str) -> None:
        self.steps.append(("copy", self._at(source), self._at(target)))

    def tree(self, source: str, target: str, empty: bool = False) -> None:
        self.steps.append(("tree", self._at(source), self._at(target), empty))

    def mkdir(self, target: str) -> None:
        self.steps.append(("mkdir", self._at(target)))

    def rmtree(self, target: str) -> None:
        self.steps.append(("rmtree", self._at(target)))

    def remove(self, target: str) -> None:
        self.steps.append(("remove", self._at(target)))

    def remove_glob(self, folder: str, pattern: str) -> None:
        self.steps.append(("remove_glob", self._at(folder), pattern))

    def write(self, target: str, lines: tuple[str, ...]) -> None:
        self.steps.append(("write", self._at(target), lines))

    def rename(self, target: str, name: str) -> None:
        self.steps.append(("rename", self._at(target), name))

    def move(self, source: str, target: str) -> None:
        self.steps.append(("move", self._at(source), self._at(target)))

    def run(self, label: str, command: list[str], cwd: str) -> None:
        self.steps.append(("run", label, [self._at(c) for c in command], self._at(cwd)))

    def action(self, name: str) -> None:
        self.steps.append(("action", name))


def _split(source: Path) -> tuple[Path, str | None]:
    if any(character in source.name for character in WILDCARD):
        return source.parent, source.name
    return source, None


def _matches(name: str, pattern: str | None) -> bool:
    return pattern is None or pattern in ANY or fnmatch.fnmatch(name.lower(), pattern.lower())


# CopyFileW is roughly twice as fast as shutil for many small files.
if WINDOWS:
    _kernel32 = ctypes.WinDLL("kernel32", use_last_error=True)
    _kernel32.CopyFileW.argtypes = (ctypes.c_wchar_p, ctypes.c_wchar_p, ctypes.c_int)
    _kernel32.CopyFileW.restype = ctypes.c_int


def _copy_file(source: str, destination: str) -> None:
    if not WINDOWS:
        shutil.copy2(source, destination)
        return
    if not _kernel32.CopyFileW(source, destination, 0):
        raise ctypes.WinError(ctypes.get_last_error())


class Result:
    """Missing sources are a content problem; blocked targets are a permissions one."""

    def __init__(self) -> None:
        self.missing: list[str] = []
        self.blocked: list[str] = []

    def __len__(self) -> int:
        return len(self.missing) + len(self.blocked)


def _ensure(folder: str, made: set) -> None:
    if folder not in made:
        os.makedirs(folder, exist_ok=True)
        made.add(folder)


def _try_copy(source: str, destination: str, result: Result) -> bool:
    try:
        _copy_file(source, destination)
        return True
    except OSError as error:
        result.blocked.append(f"{error.filename or destination} ({error.strerror or error})")
        return False


def _do_copy(source: str, target: str, result: Result,
             progress: 'Progress | None' = None) -> None:
    root, pattern = _split(Path(source))
    if pattern is None:
        if not root.is_file():
            result.missing.append(source)
            return
        destination = Path(target)
        if destination.is_dir():
            destination = destination / root.name
        os.makedirs(destination.parent, exist_ok=True)
        _try_copy(str(root), str(destination), result)
        return

    found = [item for item in sorted(root.glob("*")) if item.is_file() and _matches(item.name, pattern)]
    if not found:
        result.missing.append(source)
        return
    os.makedirs(target, exist_ok=True)
    for item in found:
        if progress is not None:
            progress.detail(item.name)
        _try_copy(str(item), os.path.join(target, item.name), result)


def _do_remove(target: str) -> None:
    root, pattern = _split(Path(target))
    if pattern is None:
        root.unlink(missing_ok=True)
        return
    for item in root.glob("*"):
        if item.is_file() and _matches(item.name, pattern):
            item.unlink()


def _do_tree(source: str, target: str, empty: bool, result: Result,
             progress: 'Progress | None' = None) -> None:
    root, pattern = _split(Path(source))
    if pattern is None and root.is_file():
        _do_copy(source, target, result, progress)
        return
    if not root.is_dir():
        result.missing.append(source)
        return

    base = str(root)
    made: set = set()
    for folder, _, names in os.walk(base):
        relative = os.path.relpath(folder, base)
        destination = target if relative == "." else os.path.join(target, relative)
        if empty:
            _ensure(destination, made)
        for name in names:
            if not _matches(name, pattern):
                continue
            _ensure(destination, made)
            if progress is not None:
                progress.detail(name)
            _try_copy(os.path.join(folder, name), os.path.join(destination, name), result)


def _execute(plan: Plan, progress: 'Progress | None' = None) -> Result:
    result = Result()
    for step in plan.steps:
        if progress is not None:
            if step[0] == "action":
                progress.pause()
            else:
                progress.step(_label(step))
        try:
            _step(step, result, progress)
        except OSError as error:
            where = error.filename or step[1]
            result.blocked.append(f"{where} ({error.strerror or error})")
    return result


def _step(step: tuple, result: Result, progress: 'Progress | None' = None) -> None:
    kind = step[0]
    if kind == "copy":
        _do_copy(step[1], step[2], result, progress)
    elif kind == "tree":
        _do_tree(step[1], step[2], step[3], result, progress)
    elif kind == "mkdir":
        Path(step[1]).mkdir(parents=True, exist_ok=True)
    elif kind == "rmtree":
        shutil.rmtree(step[1], ignore_errors=True)
    elif kind == "remove":
        _do_remove(step[1])
    elif kind == "remove_glob":
        for item in Path(step[1]).rglob(step[2]):
            item.unlink(missing_ok=True)
    elif kind == "write":
        Path(step[1]).parent.mkdir(parents=True, exist_ok=True)
        _write(Path(step[1]), step[2])
    elif kind == "rename":
        source = Path(step[1])
        if source.is_file():
            os.replace(source, source.with_name(step[2]))
        else:
            result.missing.append(step[1])
    elif kind == "move":
        source = Path(step[1])
        if not source.exists():
            result.missing.append(step[1])
        else:
            os.makedirs(Path(step[2]).parent, exist_ok=True)
            shutil.move(str(source), step[2])
    elif kind == "run":
        print(f"--- {step[1]} ---", flush=True)
        code = subprocess.run(step[2], cwd=step[3], shell=USE_SHELL).returncode
        if code:
            result.blocked.append(f"{step[1]} returned {code}")
    elif kind == "action":
        print(f"--- {step[1]} ---", flush=True)
        ACTIONS[step[1]]()


def _deployment_env(env: dict[str, str] | None) -> dict[str, str]:
    env = dict(env if env is not None else build_env())
    dev = Path(env["OPENTWIN_DEV_ROOT"])
    default = str(dev / "Deployment")
    for name in ("OT_DEPLOYMENT_DIR", "OPENTWIN_DEPLOYMENT_DIR"):
        if not env.get(name):
            env[name] = default
    if not env.get("OPENTWIN_FRONTEND_DEPLOYMENT"):
        env["OPENTWIN_FRONTEND_DEPLOYMENT"] = str(dev / "Deployment_Frontend")
    if not env.get("OPENTWIN_DEBUG_FILES"):
        env["OPENTWIN_DEBUG_FILES"] = str(dev / "DebugFiles")
    if not env.get("SYSTEM_32"):
        env["SYSTEM_32"] = str(Path(env.get("SYSTEMROOT", r"C:\Windows")) / "System32")
    return env


def _run(builder: Callable[[Plan], None], env: dict[str, str] | None) -> int:
    plan = Plan(_deployment_env(env))
    builder(plan)
    progress = Progress(sum(1 for s in plan.steps if s[0] != "action"))
    result = _execute(plan, progress)
    progress.close()
    print(SEPARATOR, flush=True)
    if result.missing:
        print(f"{len(result.missing)} source(s) do not exist:", flush=True)
        for source in result.missing:
            print(f"  {source}", flush=True)
    if result.blocked:
        print(f"{len(result.blocked)} target(s) could not be written "
              f"(existing files, permissions - the deployment kept the old copy):", flush=True)
        for target in result.blocked[:5]:
            print(f"  {target}", flush=True)
        if len(result.blocked) > 5:
            print(f"  ... and {len(result.blocked) - 5} more", flush=True)
    print(f"{len(plan.steps)} steps run, {len(result.missing)} missing, "
          f"{len(result.blocked)} unwritable", flush=True)
    return 0


def update_libraries(env: dict[str, str] | None = None) -> int:
    return _run(manifest.update_libraries, env)


def create_deployment(env: dict[str, str] | None = None) -> int:
    return _run(manifest.create_deployment, env)


def create_frontend_installer(env: dict[str, str] | None = None) -> int:
    return _run(manifest.create_frontend_installer, env)


def create_debug_files(env: dict[str, str] | None = None) -> int:
    return _run(manifest.create_debug_files, env)
