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
from typing import Callable, Sequence

from .admin import admin_panel
from .environment import Environment, certificates, lds, service_args
from .server_certificate import create_server_certificate
from .services import all_services, local, local_ui, logger, session, toolkit
from .shutdown import shutdown_all

Command = Callable[[Environment, Sequence[str]], object]

COMMANDS: dict[str, Command] = {
    "local": local,
    "local-ui": local_ui,
    "logger": logger,
    "session": session,
    "all-services": all_services,
    "admin": lambda env, argv: admin_panel(env),
    "toolkit": toolkit,
    "server-certificate": lambda env, argv: create_server_certificate(env),
    "shutdown": lambda env, argv: shutdown_all(),
}

# The set_up_*.bat files are CALLed to set variables in their caller, which a Python
# process cannot do: "setup" prints NAME=VALUE lines, the batch file sets them.
SETUP: dict[str, Command] = {
    "service-args": lambda env, argv: service_args(env),
    "certificates": lambda env, argv: certificates(env),
    "lds": lambda env, argv: lds(env, argv[0] if argv else None),
}


def _console_encoding() -> str:
    """for /f reads the output in the console code page."""
    for stream in (sys.stderr, sys.stdin):
        try:
            encoding = os.device_encoding(stream.fileno())
        except (AttributeError, OSError, ValueError):
            encoding = None
        if encoding:
            return encoding
    return "mbcs" if os.name == "nt" else "utf-8"


def _setup(part: str, argv: Sequence[str]) -> int:
    env = dict(os.environ)
    before = dict(env)
    SETUP[part](env, argv)
    encoding = _console_encoding()
    for name, value in env.items():
        if before.get(name) != value:
            sys.stdout.buffer.write(f"{name}={value}\r\n".encode(encoding, errors="replace"))
    sys.stdout.flush()
    return 0


def _usage() -> str:
    return ("usage: python -m ot_launcher <" + "|".join(COMMANDS) + "> [ARGS...]\n"
            "       python -m ot_launcher setup <" + "|".join(SETUP) + "> [dev]")


def main(argv: Sequence[str]) -> int:
    if argv[:1] == ["setup"]:
        if len(argv) < 2 or argv[1] not in SETUP:
            raise SystemExit(_usage())
        return _setup(argv[1], argv[2:])
    if not argv or argv[0] not in COMMANDS:
        raise SystemExit(_usage())

    env = dict(os.environ)
    COMMANDS[argv[0]](env, argv[1:])
    return 0
