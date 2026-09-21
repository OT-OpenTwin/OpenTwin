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

import shutil
import subprocess
import sys
from pathlib import Path
from typing import Mapping, Sequence

sys.path.insert(0, str(Path(__file__).resolve().parent))

from ot_dev import (build_admin_panel, build_all, build_documentation, build_framework,
                    build_project, cli, resolve_root)
from ot_dev.batch import Step
from ot_dev.config import order

LOG_DIR = ("Scripts", "BuildAndTest")
SUMMARY = "buildLog_Summary.txt"

# Deleted before every build, as RebuildAll.bat does.
OLD_LOGS = (SUMMARY, "RUSTbuildLog.txt", "AdminPanel_buildLog.txt",
            "Documentation_buildLog.txt", "DoxygenDocumentation_buildLog.txt")

LOCAL_CERTIFICATES = ("Certificates", "CreateLocalCertificates")
LOCAL_SCRIPT = "CreateLocalCertificates.bat"

KEY_HEADER = "OTEncryptionKey.h"
KEY_BITS = "2048"
KEY_GENERATOR = "KeyGenerator.exe"
KEY_LIBRARIES = (("OT_SYSTEM_ROOT", "OTSystem.dll"), ("OT_CORE_ROOT", "OTCore.dll"))


def create_local_certificates(env: Mapping[str, str]) -> int:
    """RebuildAll.bat:52. The script only generates when a certificate is missing."""
    folder = Path(env["OPENTWIN_DEV_ROOT"]).joinpath(*LOCAL_CERTIFICATES)
    # TODO(linux): the certificate script is a batch file.
    return subprocess.run(["cmd", "/c", LOCAL_SCRIPT], cwd=folder, env=env).returncode


def create_encryption_key(env: Mapping[str, str], key_generator: Path) -> int:
    """RebuildAll.bat:106. Only when the header is missing; it is tracked, so normally never."""
    header = Path(env["OT_ENCRYPTIONKEY_ROOT"]) / KEY_HEADER
    if header.exists():
        return 0

    print(f'Updating header file "{header}"', flush=True)
    output = key_generator / env["OT_CDLLR"]
    # OTRandom.dll, the batch's third library, no longer exists
    for root, name in KEY_LIBRARIES:
        (output / name).unlink(missing_ok=True)
        source = Path(env[root]) / env["OT_CDLLR"] / name
        if source.is_file():
            shutil.copy2(source, output)
    return subprocess.run([str(output / KEY_GENERATOR), KEY_BITS, str(header)], env=env).returncode


def _key_generator(env: dict[str, str], configs: Sequence[str], rebuild: bool, logs: Path) -> int:
    target = resolve_root(env, "KEYGENERATOR")
    step_configs, step_rebuild = order.BUILD_OVERRIDES["KEYGENERATOR"]
    code = build_project(env, target, step_configs, step_rebuild, logs)
    create_encryption_key(env, Path(target))
    return code


def _admin_panel(env: dict[str, str], configs: Sequence[str], rebuild: bool, logs: Path) -> int:
    # RebuildAll.bat:434
    if not rebuild and env.get("OT_SKIP_ADMIN_PANEL_ON_BUILD") == "true":
        print("Skipped, OT_SKIP_ADMIN_PANEL_ON_BUILD is set", flush=True)
        return 0
    return build_admin_panel(env, configs, rebuild, logs)


# Steps that are not plain CMake builds.
SPECIAL: dict[str, tuple[str, Step]] = {
    "KEYGENERATOR": ("KeyGenerator", _key_generator),
    "FRAMEWORK": ("Framework", build_framework),
    "ADMINPANEL": ("AdminPanel", _admin_panel),
}


def rebuild_all(env: dict[str, str], configurations: Sequence[str], rebuild: bool) -> int:
    logs = Path(env["OPENTWIN_DEV_ROOT"]).joinpath(*LOG_DIR)
    create_local_certificates(env)
    for name in OLD_LOGS:
        (logs / name).unlink(missing_ok=True)
    return build_all(env, order.BUILD_ORDER, order.BUILD_OVERRIDES, SPECIAL,
                     configurations, rebuild, logs, SUMMARY)


def main(argv: Sequence[str]) -> int:
    if len(argv) > 2:
        raise SystemExit("usage: build_all.py [DEBUG|RELEASE|BOTH] [BUILD|REBUILD] | --doc-only")

    env = cli.environment()
    if cli.argument(argv, 0) == "--doc-only":
        return build_documentation(env, "BOTH")
    return rebuild_all(env, cli.configurations(cli.argument(argv, 0)), cli.build_type(cli.argument(argv, 1)))


if __name__ == "__main__":
    sys.exit(cli.run(main, sys.argv[1:]))
