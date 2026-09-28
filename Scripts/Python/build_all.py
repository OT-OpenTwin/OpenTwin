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

from ot_dev import build_documentation, cli
from ot_dev.builds.rebuild import rebuild_all


def main(argv: Sequence[str]) -> int:
    if len(argv) > 2:
        raise SystemExit("usage: build_all.py [DEBUG|RELEASE|BOTH] [BUILD|REBUILD] | --doc-only")

    env = cli.environment()
    if cli.argument(argv, 0) == "--doc-only":
        return build_documentation(env, "BOTH")
    return rebuild_all(env, cli.configurations(cli.argument(argv, 0)), cli.build_type(cli.argument(argv, 1)))


if __name__ == "__main__":
    sys.exit(cli.run(main, sys.argv[1:]))
