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

from typing import Mapping

import SetupEnvironment as definitions


def project_roots() -> dict[str, str]:
    return {name.removeprefix("OT_").removesuffix("_ROOT"): name
            for group, _ in definitions.GROUPS for name in group}


def resolve_root(env: Mapping[str, str], key: str) -> str:
    roots = project_roots()
    name = roots.get(key.upper())
    if name is None or name not in env:
        raise SystemExit(f"Unknown project '{key}'. Known: " + ", ".join(sorted(roots)))
    return env[name]
