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

import sys
from pathlib import Path
from typing import Sequence

sys.path.insert(0, str(Path(__file__).resolve().parent))

from ot_dev import cli, test_all, testable_projects

LOG_DIR = ("Scripts", "BuildAndTest")


def main(argv: Sequence[str]) -> int:
    if len(argv) > 1:
        raise SystemExit("usage: test_all.py [DEBUG|RELEASE|BOTH]")

    env = cli.environment()
    configurations = cli.configurations(cli.argument(argv, 0))
    logs = Path(env["OPENTWIN_DEV_ROOT"]).joinpath(*LOG_DIR)

    # Derived, not listed: a project is tested exactly when it has tests.
    projects = testable_projects(env)
    return test_all(env, projects, configurations, logs)


if __name__ == "__main__":
    sys.exit(cli.run(main, sys.argv[1:]))
