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

import csv
import subprocess
import time
from typing import Callable

from .platform import WINDOWS

PROCESSES = ("open_twin.exe", "PythonExecution.exe", "uiFrontend.exe", "httpd.exe")

AWAITED = "httpd.exe"

_KILLED = 0
_NOT_FOUND = 128


def _pids(name: str) -> list[str]:
    result = subprocess.run(["tasklist", "/FI", f"IMAGENAME eq {name}", "/NH", "/FO", "CSV"],
                            capture_output=True, text=True, errors="replace")
    return [row[1] for row in csv.reader((result.stdout or "").splitlines())
            if len(row) > 1 and row[0].lower() == name.lower()]


def _running(name: str) -> bool:
    return bool(_pids(name))


def _require_windows() -> None:
    if not WINDOWS:
        # TODO(linux): taskkill/tasklist have no direct equivalent.
        raise SystemExit("shutdown is only implemented for Windows")


def _stop(name: str) -> tuple[str, list[str]]:
    pids = _pids(name)
    code = subprocess.run(["taskkill", "/IM", name, "/F"],
                          stdout=subprocess.DEVNULL,
                          stderr=subprocess.DEVNULL).returncode
    return {_KILLED: "stopped", _NOT_FOUND: "not running"}.get(code, f"taskkill returned {code}"), pids


def _await(report: Callable[[str], None]) -> None:
    remaining = _pids(AWAITED)
    if remaining:
        report(f"waiting for {AWAITED} to exit (pid {' '.join(remaining)})")
        while _running(AWAITED):
            time.sleep(1)


def shutdown_all() -> int:
    _require_windows()

    print("Shutting down OpenTwin", flush=True)
    for name in PROCESSES:
        state, pids = _stop(name)
        detail = f"pid {' '.join(pids)}" if pids else ""
        print(f"  {name:24}{state:14}{detail}".rstrip(), flush=True)

    _await(lambda line: print(f"  {line}", flush=True))

    print("---", flush=True)
    print("SUCCESS", flush=True)
    return 0


def stop_processes(report: Callable[[str], None]) -> list[str]:
    """shutdown_all without the report; returns only what was actually running."""
    _require_windows()

    stopped = []
    for name in PROCESSES:
        state, pids = _stop(name)
        if state == "stopped":
            stopped.append(f"{name} (pid {' '.join(pids)})" if pids else name)
        elif state != "not running":
            stopped.append(f"{name}: {state}")
    _await(report)
    return stopped
