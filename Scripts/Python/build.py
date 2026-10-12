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

from ot_dev import build_project, cli
from ot_dev.builds.wsl import ARGUMENT as WSL, build_in_wsl


def main(argv: Sequence[str]) -> int:
    # WSL is always the last word, the arguments before it keep their positions
    in_wsl = len(argv) > 1 and argv[-1].upper() == WSL
    if in_wsl:
        argv = argv[:-1]
    if not 1 <= len(argv) <= 3:
        raise SystemExit(f"usage: build.py <PROJECT> [DEBUG|RELEASE|BOTH] [BUILD|REBUILD] [{WSL}]")
    if in_wsl:
        return build_in_wsl(argv)

    env, target = cli.prepare(argv[0])
    configurations = cli.configurations(cli.argument(argv, 1))
    rebuild = cli.build_type(cli.argument(argv, 2))
    return build_project(env, target, configurations, rebuild)


if __name__ == "__main__":
    sys.exit(cli.run(main, sys.argv[1:]))
