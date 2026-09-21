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

from ot_dev import cli, run_program

OPTIONS = ("--toolchain", "--detach")


def main(argv: Sequence[str]) -> int:
    argv = list(argv)
    options = set()
    while argv and argv[0] in OPTIONS:
        options.add(argv.pop(0))
    if not argv:
        raise SystemExit("usage: run.py [--toolchain] [--detach] [NAME=VALUE ...] <PROGRAM> [ARGS...]")

    return run_program(cli.environment(), argv,
                       toolchain="--toolchain" in options, detach="--detach" in options)


if __name__ == "__main__":
    sys.exit(cli.run(main, sys.argv[1:]))
