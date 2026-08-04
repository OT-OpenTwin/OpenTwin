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

from opentwin import clean_all, cli, project_roots


def main(argv: Sequence[str]) -> int:
    if argv:
        raise SystemExit("usage: clean_all.py")

    env = cli.environment()
    return clean_all(env, sorted(project_roots()))


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
