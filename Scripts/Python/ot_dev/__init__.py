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

"""The OpenTwin developer rail.

core/        environment, configuration, shared paths and process handling
builds/      CMake projects, the framework, the admin panel, RebuildAll
deploy/      deployment plans and the running services
docs/        Sphinx and Doxygen
tools/       editors, 7-Zip, external programs, the URL protocol
installers/  the NSIS installers
"""

from .builds.admin_panel import build_admin_panel
from .builds.batch import build_all, clean_all, test_all, testable_projects
from .builds.framework import build_framework
from .builds.project import build_project, clean_project, test_project
from .core import cli  # the entry scripts use `from ot_dev import cli`
from .core.environment import build_env, check_required
from .core.process import run_program
from .core.projects import project_roots, resolve_root
from .core.toolchain import apply_toolchain
from .deploy.deployment import (create_build_information, create_debug_files, create_deployment,
                                create_frontend_installer, update_libraries)
from .deploy.services import shutdown_all
from .docs.documentation import build_documentation
from .tools.editor import launch_editor

__all__ = [
    "apply_toolchain",
    "build_admin_panel",
    "build_all",
    "build_documentation",
    "build_env",
    "build_framework",
    "build_project",
    "check_required",
    "clean_all",
    "clean_project",
    "create_build_information",
    "create_debug_files",
    "create_deployment",
    "create_frontend_installer",
    "launch_editor",
    "project_roots",
    "resolve_root",
    "run_program",
    "shutdown_all",
    "test_all",
    "test_project",
    "testable_projects",
    "update_libraries",
]
