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
import sys
import time

from ..core.platform import WINDOWS

BAR = 28

REFRESH = 0.05

LOG_ROWS = 8

ENABLE_VT = 0x0004
STD_OUTPUT_HANDLE = -11


if WINDOWS:
    _kernel32 = ctypes.WinDLL("kernel32", use_last_error=True)
    _kernel32.GetStdHandle.restype = ctypes.c_void_p
    _kernel32.GetConsoleMode.argtypes = (ctypes.c_void_p, ctypes.c_void_p)
    _kernel32.SetConsoleMode.argtypes = (ctypes.c_void_p, ctypes.c_ulong)
    _kernel32.GetConsoleScreenBufferInfo.argtypes = (ctypes.c_void_p, ctypes.c_void_p)


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
    handle = _kernel32.GetStdHandle(STD_OUTPUT_HANDLE)
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
            _kernel32.SetConsoleMode(_kernel32.GetStdHandle(STD_OUTPUT_HANDLE), mode)
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
        _kernel32.SetConsoleMode(_kernel32.GetStdHandle(STD_OUTPUT_HANDLE), self._mode)
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
