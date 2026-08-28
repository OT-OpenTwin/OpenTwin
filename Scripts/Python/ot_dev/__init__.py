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

_ROOT = str(Path(__file__).resolve().parent.parent)
if _ROOT not in sys.path:
    sys.path.insert(0, _ROOT)

from .actions import build_project, clean_project, launch_editor, test_project
from .admin_panel import build_admin_panel
from .batch import build_all, clean_all, test_all, testable_projects
from .environment import build_env, check_required
from .framework import build_framework
from .projects import project_roots, resolve_root
from .services import shutdown_all
from .toolchain import apply_toolchain

__all__ = [
    "apply_toolchain",
    "build_admin_panel",
    "build_all",
    "build_env",
    "build_framework",
    "build_project",
    "check_required",
    "clean_all",
    "clean_project",
    "launch_editor",
    "project_roots",
    "resolve_root",
    "shutdown_all",
    "test_all",
    "test_project",
    "testable_projects",
]
