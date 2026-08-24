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

from datetime import datetime
from pathlib import Path
from typing import Callable, Mapping, Sequence

from .actions import SEPARATOR, build_project, clean_project, test_project
from .cli import terminate_requested
from .projects import project_roots, resolve_root
from .toolchain import apply_toolchain

Step = Callable[[dict[str, str], Sequence[str], bool, Path], int]
Overrides = Mapping[str, tuple[Sequence[str], bool]]


def _summary(path: Path, configs: Sequence[str], logs: Path, started: datetime,
             finished: datetime, failed: Sequence[str]) -> None:
    lines = [f"Build started at: {started:%Y-%m-%d %H:%M:%S}",
             f"Build ended at:   {finished:%Y-%m-%d %H:%M:%S}", ""]
    for config in configs:
        log = logs / f"buildlog_{config.capitalize()}.txt"
        lines += [SEPARATOR, f"{config.capitalize()} Builds", SEPARATOR]
        if log.is_file():
            lines += [line for line in log.read_text(encoding="utf-8", errors="replace").splitlines()
                      if line.startswith("--- Build ")]
        lines.append("")
    lines += [SEPARATOR, "Failed projects: " + (", ".join(failed) if failed else "NONE"), SEPARATOR]
    path.write_text("\n".join(lines) + "\n", encoding="utf-8")


def _run_step(name: str, action: Callable[[], int], failed: list[str]) -> bool:
    try:
        if action():
            failed.append(name)
    except KeyboardInterrupt:
        failed.append(name)
        return terminate_requested()
    return False


def _cmake_step(key: str, target: str, overrides: Overrides) -> Step:
    def run(env: dict[str, str], configs: Sequence[str], rebuild: bool, logs: Path) -> int:
        step_configs, step_rebuild = overrides.get(key, (configs, rebuild))
        return build_project(env, target, step_configs, step_rebuild, logs)
    return run


def _resolve(env: Mapping[str, str], order: Sequence[str], overrides: Overrides,
             special: Mapping[str, tuple[str, Step]]) -> list[tuple[str, Step]]:
    steps: list[tuple[str, Step]] = []
    for key in order:
        if key in special:
            steps.append(special[key])
        else:
            target = resolve_root(env, key)
            steps.append((Path(target).name, _cmake_step(key, target, overrides)))
    return steps


def build_all(env: dict[str, str], order: Sequence[str], overrides: Overrides,
              special: Mapping[str, tuple[str, Step]], configs: Sequence[str],
              rebuild: bool, logs: str | Path, summary: str) -> int:
    logs = Path(logs)
    logs.mkdir(parents=True, exist_ok=True)
    for config in configs:
        (logs / f"buildlog_{config.capitalize()}.txt").unlink(missing_ok=True)

    env = apply_toolchain(dict(env))
    steps = _resolve(env, order, overrides, special)

    started = datetime.now()
    failed: list[str] = []
    aborted = False
    for index, (name, run) in enumerate(steps, start=1):
        print(f"\n=== [{index}/{len(steps)}] {name} ===", flush=True)
        if _run_step(name, lambda: run(env, configs, rebuild, logs), failed):
            aborted = True
            break

    finished = datetime.now()
    _summary(logs / summary, configs, logs, started, finished, failed)

    print(f"\n{SEPARATOR}", flush=True)
    if aborted:
        print("Aborted.", flush=True)
    print(f"Built {len(steps)} steps in {finished - started}", flush=True)
    print(f"Summary: {logs / summary}", flush=True)
    print("SUCCESS" if not failed else "FAILED: " + ", ".join(failed), flush=True)
    return 1 if failed else 0


def testable_projects(env: Mapping[str, str]) -> list[str]:
    """Project tokens that have tests, derived rather than listed. A project is
    testable exactly when ot_add_test() would add its tests/ subdirectory."""
    found = []
    for key in sorted(project_roots()):
        root = env.get(project_roots()[key])
        if root and (Path(root) / "tests" / "CMakeLists.txt").is_file():
            found.append(key)
    return found


def test_all(env: Mapping[str, str], projects: Sequence[str], configs: Sequence[str],
             logs: str | Path) -> int:
    logs = Path(logs)
    logs.mkdir(parents=True, exist_ok=True)
    for config in configs:
        (logs / f"testlog_{config.capitalize()}.txt").unlink(missing_ok=True)

    started = datetime.now()
    failed: list[str] = []
    aborted = False
    for index, key in enumerate(projects, start=1):
        target = resolve_root(env, key)
        name = Path(target).name
        print(f"\n=== [{index}/{len(projects)}] {name} ===", flush=True)
        if _run_step(name, lambda: test_project(env, target, configs, logs), failed):
            aborted = True
            break

    finished = datetime.now()

    print(f"\n{SEPARATOR}", flush=True)
    if aborted:
        print("Aborted.", flush=True)
    print(f"Tested {len(projects)} projects in {finished - started}", flush=True)
    print("SUCCESS" if not failed else "FAILED: " + ", ".join(failed), flush=True)
    return 1 if failed else 0


def clean_all(env: Mapping[str, str], projects: Sequence[str]) -> int:
    failed: list[str] = []
    aborted = False
    for index, key in enumerate(projects, start=1):
        target = resolve_root(env, key)
        name = Path(target).name
        print(f"\n=== [{index}/{len(projects)}] {name} ===", flush=True)
        if _run_step(name, lambda: clean_project(target), failed):
            aborted = True
            break

    print("\n---", flush=True)
    if aborted:
        print("Aborted.", flush=True)
    print("SUCCESS" if not failed else "FAILED: " + ", ".join(failed), flush=True)
    return 1 if failed else 0
