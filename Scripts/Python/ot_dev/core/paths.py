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

"""Locations and file names used by more than one part of the rail.

Tuples are path parts relative to the root named in the section.
Anything only one module needs stays in that module.
"""

from .platform import EXE, SYSTEM

# --- relative to OPENTWIN_DEV_ROOT ---
BUILD_AND_TEST = ("Scripts", "BuildAndTest")
INSTALLER = ("Scripts", "Installer")
LAUNCHER = ("Scripts", "Launcher")
DEPLOYMENT = ("Deployment",)
DEPLOYMENT_FRONTEND = ("Deployment_Frontend",)
DEBUG_FILES = ("DebugFiles",)
DEVELOPER_DOCS = ("Documentation", "Developer")
BUILD_INFO = ("Deployment", "BuildInfo.txt")

# --- log files in BUILD_AND_TEST ---
SUMMARY = "buildLog_Summary.txt"
FRAMEWORK_LOG = "Framework_buildLog.txt"
ADMIN_PANEL_LOG = "AdminPanel_buildLog.txt"
SPHINX_LOG = "Documentation_buildLog.txt"
DOXYGEN_LOG = "DoxygenDocumentation_buildLog.txt"


def build_log(config: str) -> str:
    return f"buildLog_{config.capitalize()}.txt"


def test_log(config: str) -> str:
    return f"testlog_{config.capitalize()}.txt"


# --- relative to a project root ---
def cmake_output(config: str) -> tuple[str, ...]:
    return ("build", f"{SYSTEM}-{config}", config.capitalize())


def cmake_tests(config: str) -> tuple[str, ...]:
    return ("build", f"{SYSTEM}-{config}", "tests")


RELEASE_OUTPUT = cmake_output("release")

# --- relative to OPENTWIN_THIRDPARTY_ROOT ---
# TODO(linux): the ThirdParty 7-Zip is the Windows build.
SEVEN_ZIP = ("7-Zip", "Win64", "7z" + EXE)
