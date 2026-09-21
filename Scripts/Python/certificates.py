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
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path
from typing import Mapping, Sequence

sys.path.insert(0, str(Path(__file__).resolve().parent))

from ot_dev import cli
from ot_dev.platform import WINDOWS

SOURCE = ("Certificates", "CreateLocalCertificates")
GENERATED = ("Certificates", "Generated")

CERTIFICATES = ("ca.pem", "ca-key.pem", "server.pem", "server-key.pem", "certificateKeyFile.pem")
KEY_FILE = "certificateKeyFile.pem"

# TODO(linux): ThirdParty/CertificateCreation only ships the Windows binaries.
TOOLS = ("cfssl", "cfssljson")
SUFFIX = ".exe" if WINDOWS else ""

KEY_HEADER = "OTEncryptionKey.h"
KEY_BITS = "2048"
KEY_GENERATOR = "KeyGenerator" + SUFFIX
# TODO(linux): shared libraries are found through the rpath there, not copied next to the tool.
KEY_LIBRARIES = (("OT_SYSTEM_ROOT", "OTSystem.dll"), ("OT_CORE_ROOT", "OTCore.dll"))


def _gencert(tools: Mapping[str, Path], work: Path, arguments: Sequence[str], bare: str) -> bool:
    """cfssl gencert <arguments> | cfssljson -bare <bare>"""
    cfssl = subprocess.run([str(tools["cfssl"]), "gencert", *arguments], cwd=work, stdout=subprocess.PIPE)
    if cfssl.returncode:
        return False
    return subprocess.run([str(tools["cfssljson"]), "-bare", bare], cwd=work, input=cfssl.stdout).returncode == 0


def create_local_certificates(env: Mapping[str, str]) -> int:
    dev = Path(env["OPENTWIN_DEV_ROOT"])
    source = dev.joinpath(*SOURCE)
    generated = dev.joinpath(*GENERATED)
    if not generated.is_dir():
        print(f"{generated} is missing. It is tracked in git, restore it with: "
              "git checkout -- Certificates/Generated", file=sys.stderr, flush=True)
        return 1
    if all((generated / name).is_file() for name in CERTIFICATES):
        return 0

    for name in CERTIFICATES:
        (generated / name).unlink(missing_ok=True)

    tools = {name: Path(env.get("CERT_CREATE_TOOLS", "")) / (name + SUFFIX) for name in TOOLS}
    missing = [str(tool) for tool in tools.values() if not tool.is_file()]
    if missing:
        print("Certificate tools not found: " + ", ".join(missing), file=sys.stderr, flush=True)
        return 1

    with tempfile.TemporaryDirectory(prefix="ot_certificates_") as folder:
        work = Path(folder)
        created = (_gencert(tools, work, ["-initca", str(source / "ca-csr.json")], "ca")
                   and _gencert(tools, work, ["-ca=ca.pem", "-ca-key=ca-key.pem",
                                              f"-config={source / 'ca-config.json'}", "-profile=server",
                                              str(source / "server-csr.json")], "server"))
        absent = [name for name in CERTIFICATES if name != KEY_FILE and not (work / name).is_file()]
        if not created or absent:
            print("Certificate generation failed, see the cfssl output above.", file=sys.stderr, flush=True)
            return 1

        (work / KEY_FILE).write_bytes((work / "server.pem").read_bytes() + (work / "server-key.pem").read_bytes())
        for name in CERTIFICATES:
            shutil.copyfile(work / name, generated / name)

    print("SUCCESS: The certificates have been created successfully.", flush=True)
    return 0


def create_encryption_key(env: Mapping[str, str], key_generator: Path) -> int:
    header = Path(env["OT_ENCRYPTIONKEY_ROOT"]) / KEY_HEADER
    if header.exists():
        return 0

    print(f'Updating header file "{header}"', flush=True)
    output = key_generator / env["OT_CDLLR"]
    for root, name in KEY_LIBRARIES:
        (output / name).unlink(missing_ok=True)
        source = Path(env[root]) / env["OT_CDLLR"] / name
        if source.is_file():
            shutil.copy2(source, output)
    return subprocess.run([str(output / KEY_GENERATOR), KEY_BITS, str(header)], env=env).returncode


def main(argv: Sequence[str]) -> int:
    if argv:
        raise SystemExit("usage: certificates.py")

    return create_local_certificates(os.environ)


if __name__ == "__main__":
    sys.exit(cli.run(main, sys.argv[1:]))
