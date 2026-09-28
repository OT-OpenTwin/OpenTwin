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
import sys
from pathlib import Path
from typing import Sequence

sys.path.insert(0, str(Path(__file__).resolve().parent))

from ot_dev import cli
from ot_dev.installers.nsis import build_installers, build_upgrader, create_installer_build_information

COMMANDS = {
    "build": build_installers,
    "build-information": create_installer_build_information,
    "upgrader": build_upgrader,
}


def main(argv: Sequence[str]) -> int:
    if len(argv) != 1 or argv[0] not in COMMANDS:
        raise SystemExit("usage: installers.py <" + "|".join(COMMANDS) + ">")

    env = os.environ if argv[0] == "build-information" else cli.environment()
    return COMMANDS[argv[0]](env)


if __name__ == "__main__":
    sys.exit(cli.run(main, sys.argv[1:]))
