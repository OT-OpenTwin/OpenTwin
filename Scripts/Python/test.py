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

from ot_build import cli, test_project


def main(argv: Sequence[str]) -> int:
    if not 1 <= len(argv) <= 2:
        raise SystemExit("usage: test.py <PROJECT> [DEBUG|RELEASE|BOTH]")

    env, target = cli.prepare(argv[0])
    configurations = cli.configurations(cli.argument(argv, 1))
    return test_project(env, target, configurations)


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
