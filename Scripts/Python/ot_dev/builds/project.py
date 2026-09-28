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
import re
import shutil
import subprocess
from pathlib import Path
from typing import Mapping, Sequence, TextIO

from ..core import paths
from ..core.output import SEPARATOR, build_result, finish
from ..core.platform import ENV_VARS, SYSTEM, cmake_executable, ctest_executable
from ..core.toolchain import apply_toolchain

CLEAN_DIRS = [".vs", "build", "x64", "packages", "test"]

TEST_DLL_PATHS = {"debug": "OT_ALL_DLLD", "release": "OT_ALL_DLLR"}


def _build_config(cmake: Path, env: Mapping[str, str], target: str, config: str,
                  rebuild: bool, parallel: Sequence[str], out: TextIO) -> int:
    def run(*args: str) -> subprocess.CompletedProcess[bytes]:
        return subprocess.run([str(cmake), *args], cwd=target, env=env,
                              stdout=out, stderr=subprocess.STDOUT)

    run("--preset", f"{SYSTEM}-{config}")
    if rebuild:
        run("--build", "--preset", f"build-{SYSTEM}-{config}", "--target", "clean")
    return run("--build", "--preset", f"build-{SYSTEM}-{config}", *parallel).returncode


def build_project(env: Mapping[str, str], target: str, configs: Sequence[str],
                  rebuild: bool, logs: str | Path | None = None) -> int:
    cmake = cmake_executable(env)
    env = apply_toolchain(dict(env))
    env.update(ENV_VARS)
    parallel = ["--parallel"] if env.get("OPENTWIN_DEV_PARALLEL_BUILDS") else []
    logs = Path(logs) if logs else Path.cwd()
    failed = 0

    print(f"Building Project {target}", flush=True)
    for config in configs:
        print(config.upper(), flush=True)
        with open(logs / paths.build_log(config), "a", encoding="utf-8") as out:
            out.write(f"{SEPARATOR}\nBuilding project: {target}\n{SEPARATOR}\n")
            out.flush()
            code = _build_config(cmake, env, target, config, rebuild, parallel, out)
            out.write(build_result(target, code))
        failed = failed or code

    finish(failed)
    return failed


_PASSED = re.compile(r"\[  PASSED  \] (\d+) test")
_FAILED = re.compile(r"\[  FAILED  \] ([A-Za-z_][\w/]*\.[\w/]+)")
_NO_RUN = re.compile(r"(Exit code 0x[0-9a-fA-F]+|Subprocess aborted|Timeout)")


def _test_summary(output: str) -> str:
    passed = sum(int(n) for n in _PASSED.findall(output))
    failed = list(dict.fromkeys(_FAILED.findall(output)))

    if not passed and not failed:
        stopped = _NO_RUN.search(output)
        return f"  did not run ({stopped.group(1)})" if stopped else "  no tests reported"

    counts = f"  {passed} passed" + (f", {len(failed)} failed" if failed else "")
    return "\n".join([counts] + [f"    {name}" for name in failed])


def _test_config(ctest: Path, env: Mapping[str, str], target: str, config: str,
                 out: TextIO) -> int | None:
    tests = Path(target).joinpath(*paths.cmake_tests(config))
    if not tests.is_dir():
        out.write(f"--- Not built, nothing to test: {tests} ---\n")
        return None

    result = subprocess.run(
        [str(ctest), "-C", config.capitalize(), "--test-dir", str(tests), "-V"],
        cwd=target, env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
        text=True, errors="replace")

    out.write(result.stdout)
    print(_test_summary(result.stdout), flush=True)
    return result.returncode


def test_project(env: Mapping[str, str], target: str, configs: Sequence[str],
                 logs: str | Path | None = None) -> int:
    ctest = ctest_executable(env)
    logs = Path(logs) if logs else Path.cwd()
    failed = 0
    ran = 0

    print(f"Testing Project {target}", flush=True)
    for config in configs:
        print(config.upper(), flush=True)

        step = dict(env)
        dlls = step.get(TEST_DLL_PATHS[config], "")
        if dlls:
            step["PATH"] = dlls + os.pathsep + step.get("PATH", "")

        with open(logs / paths.test_log(config), "a", encoding="utf-8") as out:
            out.write(f"{SEPARATOR}\nTesting project: {target}\n{SEPARATOR}\n")
            out.flush()
            code = _test_config(ctest, step, target, config, out)
            if code is None:
                out.write(f"--- Test skipped: {target} ---\n")
            else:
                ran += 1
                out.write(f"--- Test {'successful' if code == 0 else 'failed'}: {target} ---\n")
                failed = failed or code

    if not ran:
        print("---", flush=True)
        print("SKIPPED (not built)", flush=True)
    else:
        finish(failed)
    return failed


def clean_project(target: str | Path) -> int:
    if not Path(target).is_dir():
        raise SystemExit(f"path does not exist: {target}")

    print(f"Cleaning Project {target}", flush=True)
    locked: list[str] = []
    for name in CLEAN_DIRS:
        path = Path(target) / name
        if not path.is_dir():
            continue
        shutil.rmtree(path, ignore_errors=True)
        if path.exists():
            locked.append(name)
        print(f"{name}{' (locked)' if path.exists() else ''}", flush=True)

    finish(bool(locked))
    return 1 if locked else 0
