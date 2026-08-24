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

from typing import Mapping, Sequence, TypeVar

from .environment import build_env
from .projects import resolve_root

T = TypeVar("T")

CONFIGURATIONS: dict[str, list[str]] = {
    "DEBUG": ["debug"],
    "RELEASE": ["release"],
    "BOTH": ["debug", "release"],
}
BUILD_TYPES: dict[str, bool] = {"BUILD": False, "REBUILD": True}

DEFAULT_CONFIGURATION = "BOTH"
DEFAULT_BUILD_TYPE = "REBUILD"


def _choice(table: Mapping[str, T], value: str | None, default: str, label: str) -> T:
    key = (value or default).upper()
    if key not in table:
        raise SystemExit(f"Unknown {label} '{value}'. Known: " + ", ".join(table))
    return table[key]


def environment() -> dict[str, str]:
    return build_env()


def prepare(project: str) -> tuple[dict[str, str], str]:
    env = build_env()
    return env, resolve_root(env, project)


def configurations(value: str | None) -> list[str]:
    return _choice(CONFIGURATIONS, value, DEFAULT_CONFIGURATION, "configuration")


def build_type(value: str | None) -> bool:
    return _choice(BUILD_TYPES, value, DEFAULT_BUILD_TYPE, "build type")


def argument(argv: Sequence[str], index: int) -> str | None:
    return argv[index] if len(argv) > index else None
