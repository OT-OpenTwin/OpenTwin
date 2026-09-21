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

import contextlib
import ctypes
import fnmatch
import io
import os
import shutil
import subprocess
import sys
import time
from datetime import datetime
from pathlib import Path
from typing import Callable

from .actions import SEPARATOR
from .config import definitions, manifest
from .environment import build_env
from .expansion import expand
from .platform import USE_SHELL, WINDOWS
from .services import stop_processes

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

LOG_ROWS = 8

ENABLE_VT = 0x0004


class _Coord(ctypes.Structure):
    _fields_ = [("x", ctypes.c_short), ("y", ctypes.c_short)]


class _Rect(ctypes.Structure):
    _fields_ = [("left", ctypes.c_short), ("top", ctypes.c_short),
                ("right", ctypes.c_short), ("bottom", ctypes.c_short)]


class _BufferInfo(ctypes.Structure):
    _fields_ = [("size", _Coord), ("cursor", _Coord), ("attributes", ctypes.c_ushort),
                ("window", _Rect), ("maximum", _Coord)]


def _console() -> tuple[int, int, int, int] | None:
    """Cursor row, window height and width plus the previous mode, once VT sequences are on."""
    # TODO(linux): no pinned bar yet, the plain output is used instead.
    if not WINDOWS or not sys.stdout.isatty():
        return None
    handle = _kernel32.GetStdHandle(-11)
    mode = ctypes.c_ulong()
    info = _BufferInfo()
    if not _kernel32.GetConsoleMode(handle, ctypes.byref(mode)):
        return None
    if not _kernel32.SetConsoleMode(handle, mode.value | ENABLE_VT):
        return None
    if not _kernel32.GetConsoleScreenBufferInfo(handle, ctypes.byref(info)):
        _kernel32.SetConsoleMode(handle, mode.value)
        return None
    window = info.window
    return (info.cursor.y - window.top + 1, window.bottom - window.top + 1,
            window.right - window.left + 1, mode.value)


class Progress:
    """Bar pinned above a scrolling log in a console; plain lines when redirected."""

    def __init__(self, total: int) -> None:
        self.total = max(total, 1)
        self.done = 0
        self.label = ""
        self.started = time.monotonic()
        self.row = 0
        self._last = 0.0
        self._mode = None
        console = _console()
        if console:
            self._pin(*console)

    def _pin(self, row: int, height: int, width: int, mode: int) -> None:
        if height < LOG_ROWS + 3:
            _kernel32.SetConsoleMode(_kernel32.GetStdHandle(-11), mode)
            return
        # scroll up if needed so at least LOG_ROWS lines stay free under the bar
        row -= max(0, row + LOG_ROWS - height)
        self.row, self.width, self._mode = row, width, mode
        sys.stdout.write("\n" * LOG_ROWS + f"\033[{LOG_ROWS}A"
                         f"\033[{row + 1};{height}r\033[{row + 1};1H")
        self._draw("")

    def step(self, label: str) -> None:
        self.done += 1
        self.label = label
        self._draw(label)

    def detail(self, name: str) -> None:
        # throttled by time so naming every file costs nothing
        if not self.row:
            return
        now = time.monotonic()
        if now - self._last < REFRESH:
            return
        self._last = now
        self._draw(f"{self.label}: {name}")

    def note(self, title: str, text: str) -> None:
        print(f"  {title:<10}{text}", flush=True)

    def close(self) -> None:
        if not self.row:
            return
        self._draw("done")
        sys.stdout.write("\0337\033[r\0338")
        sys.stdout.flush()
        _kernel32.SetConsoleMode(_kernel32.GetStdHandle(-11), self._mode)
        self.row = 0

    def _draw(self, label: str) -> None:
        filled = BAR * self.done // self.total
        bar = "#" * filled + "." * (BAR - filled)
        seconds = int(time.monotonic() - self.started)
        line = f"  [{bar}] {self.done:3}/{self.total} {seconds // 60:2}:{seconds % 60:02}  {label:<40.40}"
        if self.row:
            sys.stdout.write(f"\0337\033[{self.row};1H\033[2K{line[:self.width - 1]}\0338")
            sys.stdout.flush()
        else:
            print(line, flush=True)


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


def _finished(step: tuple, base: Path, result: 'Result', copied: int, missing: int,
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
    return True, str(Path(*BUILD_INFO))


def _build_all(progress: Progress) -> tuple[bool, str]:
    """CreateDebugFiles.bat calls BuildAll.bat, which is RebuildAll.bat BOTH BUILD."""
    import build_all as script

    from . import cli

    env = build_env()
    logs = Path(env["OPENTWIN_DEV_ROOT"]).joinpath(*script.LOG_DIR)
    code = script.rebuild_all(env, cli.configurations("BOTH"), cli.build_type("BUILD"))
    return code == 0, f"build all returned {code}, see {logs}"


# name: (title, function, streams its own output)
ACTIONS = {"shutdown": ("Stopped", _shutdown, False),
           "build information": ("Written", _build_information, False),
           "build all": ("Build all", _build_all, True)}


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
    _kernel32.GetStdHandle.restype = ctypes.c_void_p
    _kernel32.GetConsoleMode.argtypes = (ctypes.c_void_p, ctypes.c_void_p)
    _kernel32.SetConsoleMode.argtypes = (ctypes.c_void_p, ctypes.c_ulong)
    _kernel32.GetConsoleScreenBufferInfo.argtypes = (ctypes.c_void_p, ctypes.c_void_p)


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
        self.failed: list[str] = []
        self.copied = 0

    def __len__(self) -> int:
        return len(self.missing) + len(self.blocked) + len(self.failed)


def _ensure(folder: str, made: set) -> None:
    if folder not in made:
        os.makedirs(folder, exist_ok=True)
        made.add(folder)


def _try_copy(source: str, destination: str, result: Result) -> bool:
    try:
        _copy_file(source, destination)
        result.copied += 1
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


def _execute(plan: Plan, progress: Progress | None = None) -> Result:
    result = Result()
    progress = progress or Progress(0)
    base = Path(plan.env.get("OT_DEPLOYMENT_DIR", ".")).parent
    notes: set = set()
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


def _action(name: str, progress: Progress, result: Result, notes: set) -> None:
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
