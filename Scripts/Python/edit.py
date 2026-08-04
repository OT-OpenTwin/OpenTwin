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

from opentwin import launch_editor, cli


def main(argv: Sequence[str]) -> int:
    if not 1 <= len(argv) <= 2:
        raise SystemExit("usage: edit.py <PROJECT> [EDITOR]")

    env, target = cli.prepare(argv[0])
    return launch_editor(env, target, cli.argument(argv, 1))


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
