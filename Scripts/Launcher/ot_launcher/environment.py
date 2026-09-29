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

import json
import os
import re
import sys
from typing import MutableMapping

from .service_args import DEFAULTS

# the folder with the package, the Deployment root in an installation
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

CERTIFICATES_PATH = "OPEN_TWIN_CERTS_PATH"
CERTIFICATES_FOLDER = "Certificates"
CERTIFICATES_DONE = "OT_CERT_SETUP_COMPLETED"
CERTIFICATES = (
    ("OPEN_TWIN_CA_CERT", "ca.pem"),
    ("OPEN_TWIN_CERT_KEY", "certificateKeyFile.pem"),
    ("OPEN_TWIN_SERVER_CERT", "server.pem"),
    ("OPEN_TWIN_SERVER_CERT_KEY", "server-key.pem"),
)

LDS_CONFIGURATION = "OT_LOCALDIRECTORYSERVICE_CONFIGURATION"
LDS_DEFINED = "OT_LOCALDIRECTORYSERVICE_CONFIGURATION_DEFINED"
LDS_ROOT = "OT_BATCH_TMP"
LDS_DEV = "dev"
LDS_DEV_MISSING = ('Please specify the following environmentvariable when using the "dev" argument '
                   "for the set up LDS batch: OPENTWIN_DEV_ROOT")
# TODO(linux): the launcher is open_twin.exe.
LAUNCHER = "open_twin.exe"
LDS_SETTINGS = {
    "DefaultMaxCrashRestarts": 8,
    "DefaultMaxStartupRestarts": 64,
    "ServiceStartWorkerCount": 4,
    "InitializeWorkerCount": 12,
}
SUPPORTED_SERVICES = [
    "CartesianMeshService",
    "FITTDService",
    "KrigingService",
    "Model",
    "ModelingService",
    "PHREECService",
    "RelayService",
    "TetMeshService",
    "ImportParameterizedDataService",
    "VisualizationService",
    "PythonExecutionService",
    "GetDPService",
    "ElmerFEMService",
    "DataProcessingService",
    "DebugService",
    "CircuitSimulatorService",
    "StudioSuiteService",
    "LTSpiceService",
    "PyritService",
    "HierarchicalProjectService",
    "FileManagementProjectService",
    "OpenEMSService",
]

Environment = MutableMapping[str, str]

VARIABLE = re.compile(r"%([^%]+)%")


def get(env: Environment, name: str) -> str:
    """Case-insensitive like cmd; an empty value counts as not set."""
    if name in env:
        return env[name]
    low = name.lower()
    return next((value for key, value in env.items() if key.lower() == low), "")


def expand(env: Environment, text: str) -> str:
    return VARIABLE.sub(lambda match: get(env, match.group(1)), text)


def service_args(env: Environment) -> None:
    for name, template in DEFAULTS.items():
        if not get(env, name):
            env[name] = expand(env, template)


def certificates(env: Environment) -> None:
    if get(env, CERTIFICATES_DONE):
        return
    folder = get(env, CERTIFICATES_PATH)
    for name, filename in CERTIFICATES:
        env[name] = f"{folder}\\{filename}" if folder else f"{CERTIFICATES_FOLDER}\\{filename}"
    env[CERTIFICATES_DONE] = "1"


def lds(env: Environment, mode: str | None = None, root: str | None = None) -> None:
    if get(env, LDS_DEFINED):
        return
    if mode == LDS_DEV:
        dev = get(env, "OPENTWIN_DEV_ROOT")
        if not dev:
            print(LDS_DEV_MISSING, file=sys.stderr, flush=True)
            return
        root = dev + "\\Deployment"
    else:
        root = root or ROOT + os.sep
    env[LDS_ROOT] = root.replace("\\", "\\\\")
    env[LDS_CONFIGURATION] = json.dumps({
        **LDS_SETTINGS,
        "ServicesLibraryPath": root,
        "LauncherPath": root + "\\" + LAUNCHER,
        "SupportedServices": SUPPORTED_SERVICES,
    })
    env[LDS_DEFINED] = "1"


def services(env: Environment, mode: str | None = None) -> None:
    service_args(env)
    lds(env, mode)
