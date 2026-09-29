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

import locale
import os
import shutil
import subprocess
import sys

from .environment import CERTIFICATES_PATH, Environment, get, services

TEMPLATE = "server-csr_template.json"
REQUEST = "server-csr.json"
PLACEHOLDERS = (("$HOSTNAME$", "COMPUTERNAME"), ("$IP_ADDRESS$", "OPEN_TWIN_SERVICES_ADDRESS"))
GENCERT = ["gencert", "-ca=ca.pem", "-ca-key=ca-key.pem", "-config=ca-config.json", "-profile=server", REQUEST]
BARE = "server"
KEY_FILE = "certificateKeyFile.pem"
KEY_PARTS = ("server.pem", "server-key.pem")
INSTALLED = ("ca.pem", "server.pem", "server-key.pem", KEY_FILE)

NOT_FOUND = "The system cannot find the file specified."


def _program(name: str) -> str:
    """The current directory first, then PATH, like cmd."""
    # TODO(linux): the certificate tools are Windows executables.
    local = os.path.join(os.getcwd(), name + ".exe")
    return local if os.path.isfile(local) else (shutil.which(name) or name)


def _copy(source: str, target: str) -> None:
    """Prints what cmd's COPY /Y prints."""
    if os.path.isdir(target):
        target = os.path.join(target, os.path.basename(source))
    if not os.path.isfile(source):
        print(NOT_FOUND, flush=True)
        return
    if os.path.exists(target) and os.path.samefile(source, target):
        print("The file cannot be copied onto itself.", flush=True)
        print("        0 file(s) copied.", flush=True)
        return
    shutil.copy2(source, target)
    print("        1 file(s) copied.", flush=True)


def _replace(path: str, placeholder: str, value: str) -> None:
    """Prints what fart prints."""
    encoding = locale.getpreferredencoding(False)
    with open(path, "rb") as handle:
        content = handle.read()
    old, new = placeholder.encode(encoding), value.encode(encoding)
    count = content.count(old)
    if count:
        with open(path, "wb") as handle:
            handle.write(content.replace(old, new))
        print(path, flush=True)
    print(f"Replaced {count} occurence(s) in {1 if count else 0} file(s).", flush=True)


def _generate(env: Environment) -> None:
    """cfssl gencert ... | cfssljson -bare server"""
    request = subprocess.run([_program("cfssl"), *GENCERT], stdout=subprocess.PIPE, env=dict(env))
    subprocess.run([_program("cfssljson"), "-bare", BARE], input=request.stdout, env=dict(env))


def _join_key_file() -> None:
    """The file names go to stderr, as with cmd's TYPE of several files."""
    with open(KEY_FILE, "wb") as out:
        for part in KEY_PARTS:
            print(f"\n{part}\n\n", file=sys.stderr, flush=True)
            if os.path.isfile(part):
                with open(part, "rb") as handle:
                    out.write(handle.read())
            else:
                print(NOT_FOUND, file=sys.stderr, flush=True)


def create_server_certificate(env: Environment) -> int:
    folder = get(env, CERTIFICATES_PATH)
    if not folder:
        print(f"ERROR: Please specify {CERTIFICATES_PATH} pointing to an existing directory", flush=True)
        return 0

    services(env)
    _copy(TEMPLATE, REQUEST)
    for placeholder, name in PLACEHOLDERS:
        _replace(REQUEST, placeholder, get(env, name))
    _generate(env)
    _join_key_file()

    if os.path.isdir(folder):
        print(f"A subdirectory or file {folder} already exists.", file=sys.stderr, flush=True)
    else:
        try:
            os.makedirs(folder)
        except OSError as error:
            print(error, file=sys.stderr, flush=True)
    for name in INSTALLED:
        _copy(name, folder)

    print("SUCCESS: The certificates have been created successfully.", flush=True)
    return 0
