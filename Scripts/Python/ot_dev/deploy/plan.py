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
from pathlib import Path

from ..core.expansion import expand
from ..core.platform import WINDOWS
from .progress import Progress

WILDCARD = ("*", "?")

ANY = ("*.*", "*")


class Plan:
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
    def __init__(self) -> None:
        self.missing: list[str] = []
        self.blocked: list[str] = []
        self.failed: list[str] = []
        self.copied = 0

    def __len__(self) -> int:
        return len(self.missing) + len(self.blocked) + len(self.failed)


def _ensure(folder: str, made: set[str]) -> None:
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
             progress: Progress | None = None) -> None:
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
             progress: Progress | None = None) -> None:
    root, pattern = _split(Path(source))
    if pattern is None and root.is_file():
        _do_copy(source, target, result, progress)
        return
    if not root.is_dir():
        result.missing.append(source)
        return

    base = str(root)
    made: set[str] = set()
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


def _write(target: Path, lines) -> None:
    with open(target, "w", newline="") as handle:
        handle.writelines(f"{line}\r\n" for line in lines)


def _step(step: tuple, result: Result, progress: Progress | None = None) -> None:
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
