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

"""Starting OpenTwin: the services, the admin panel, the toolkit and the frontend."""

from typing import NamedTuple, Sequence

from .admin import admin_panel
from .console import start, verbose
from .environment import LAUNCHER, Environment, certificates, get, services


def _address(name: str) -> str:
    return f"%OPEN_TWIN_SERVICES_ADDRESS%:%OPEN_TWIN_{name}_PORT%"


# where the services are reachable
LOG, AUTH, GSS, LSS, GDS, LDS, LMS, TOOLKIT_ADDRESS = (
    _address(name) for name in ("LOG", "AUTH", "GSS", "LSS", "GDS", "LDS", "LMS", "TOOLKIT"))

LOGGING = "%OPEN_TWIN_LOGGING_URL%"
MONGODB = "%OPEN_TWIN_MONGODB_ADDRESS%"
PASSWORD = "%OPEN_TWIN_MONGODB_PWD%"
AUTH_PORT = "%OPEN_TWIN_AUTH_PORT%"
DOWNLOAD_PORT = "%OPEN_TWIN_DOWNLOAD_PORT%"
TOOLKIT_ARGS = "OPEN_TWIN_OTOOLKIT_ARGS"


class Service(NamedTuple):
    title: str
    library: str
    arguments: tuple[str, ...]
    # the Library Management Service always had a space before the /V pause
    gap: str = ""


def _service(title: str, library: str, *arguments: str, gap: str = "") -> Service:
    return Service(title, library, arguments, gap)


# window title, service DLL, arguments of open_twin.exe
LOGGER = _service("OPEN TWIN LOGGER", "LoggerService.dll", "", LOG, "", "")

SESSION = (
    _service("AUTHORIZATION SERVICE",      "AuthorisationService.dll",     AUTH, MONGODB, PASSWORD, LOGGING),
    _service("GLOBAL SESSION SERVICE",     "GlobalSessionService.dll",     LOGGING, GSS, MONGODB, AUTH, DOWNLOAD_PORT),
    _service("LOCAL SESSION SERVICE",      "LocalSessionService.dll",      LOGGING, LSS, GSS, AUTH_PORT),
    _service("GLOBAL DIRECTORY SERVICE",   "GlobalDirectoryService.dll",   LOGGING, GDS, GSS, "unused"),
    _service("LOCAL DIRECTORY SERVICE",    "LocalDirectoryService.dll",    LOGGING, LDS, GDS, AUTH_PORT),
    _service("LIBRARY MANAGEMENT SERVICE", "LibraryManagementService.dll", LOGGING, LMS, GSS, PASSWORD, gap=" "),
)

TOOLKIT = _service("OPEN TWIN TOOLKIT SERVICE", "OToolkit.dll", "", TOOLKIT_ADDRESS, "", f"%{TOOLKIT_ARGS}%")
LOG_EXPORT = "-logexport"
NO_AUTO = "-noauto"

FRONTEND = "START /B uiFrontend.exe"


def command_line(service: Service) -> str:
    """The START line: open_twin.exe gets the service DLL and its quoted arguments."""
    arguments = " ".join(f'"{argument}"' for argument in service.arguments)
    return (f'START "{service.title}" %pause_prefix%{LAUNCHER} {service.library} '
            f'{arguments}{service.gap}%pause_suffix%')


def _prepare(env: Environment, argv: Sequence[str]) -> None:
    certificates(env)
    services(env)
    verbose(env, argv)


def logger(env: Environment, argv: Sequence[str]) -> None:
    _prepare(env, argv)
    start(env, command_line(LOGGER))


def session(env: Environment, argv: Sequence[str]) -> None:
    _prepare(env, argv)
    for service in SESSION:
        start(env, command_line(service))
    admin_panel(env)


def _toolkit_arguments(env: Environment, argv: Sequence[str]) -> None:
    if get(env, TOOLKIT_ARGS):
        return
    first, second = (list(argv) + ["", ""])[:2]
    if LOG_EXPORT in (first, second):
        env[TOOLKIT_ARGS] = LOG_EXPORT
    for argument in (first, second):
        if argument == NO_AUTO:
            env[TOOLKIT_ARGS] = get(env, TOOLKIT_ARGS) + " " + NO_AUTO


def toolkit(env: Environment, argv: Sequence[str]) -> None:
    _prepare(env, argv)
    _toolkit_arguments(env, argv)
    start(env, command_line(TOOLKIT))


def frontend(env: Environment) -> None:
    start(env, FRONTEND)


def local(env: Environment, argv: Sequence[str]) -> None:
    logger(env, argv)
    session(env, argv)
    frontend(env)


def local_ui(env: Environment, argv: Sequence[str]) -> None:
    certificates(env)
    services(env)
    frontend(env)


def all_services(env: Environment, argv: Sequence[str]) -> None:
    logger(env, argv)
    session(env, argv)
