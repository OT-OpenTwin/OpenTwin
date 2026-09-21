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

"""Order in which build_all.py builds the projects.

Add a new project's key here, after everything it depends on.
"""

# Dependency order, mirroring Scripts\BuildAndTest\RebuildAll.bat
BUILD_ORDER: list[str] = [
    "SYSTEM",
    "CORE",
    "KEYGENERATOR",
    "GUI",
    "COMMUNICATION",
    "GUIAPI",
    "WIDGETS",
    "DATASTORAGE",
    "MODELENTITIES",
    "CADMODELENTITIES",
    "BLOCKENTITIES",
    "MODELAPI",
    "FOUNDATION",
    "UICORE",
    "RUBBERBANDAPI",
    "RUBBERBAND_OSG",
    "VIEWER",
    "RESULT_DATA_ACCESS",
    "FRONTEND_CONNECTOR_API",
    "FILE_MANAGER_CONNECTOR",
    "STUDIO_SUITE_CONNECTOR",
    "LTSPICE_CONNECTOR",
    "FRAMEWORK",
    "MODEL_SERVICE",
    "GLOBAL_SESSION_SERVICE",
    "LOCAL_SESSION_SERVICE",
    "GLOBAL_DIRECTORY_SERVICE",
    "LOCAL_DIRECTORY_SERVICE",
    "RELAY_SERVICE",
    "LOGGER_SERVICE",
    "AUTHORISATION_SERVICE",
    "PHREEC_SERVICE",
    "MODELING_SERVICE",
    "VISUALIZATION_SERVICE",
    "FITTD_SERVICE",
    "CARTESIAN_MESH_SERVICE",
    "TET_MESH_SERVICE",
    "IMPORT_PARAMETERIZED_DATA_SERVICE",
    "GETDP_SERVICE",
    "ELMERFEM_SERVICE",
    "STUDIOSUITE_SERVICE",
    "LTSPICE_SERVICE",
    "PYRIT_SERVICE",
    "OPENEMS_SERVICE",
    "PYTHON_EXECUTION_SERVICE",
    "PYTHON_EXECUTION",
    "DEBUGSERVICE",
    "DATA_PROCESSING_SERVICE",
    "CIRCUIT_SIMULATOR_SERVICE",
    "CIRCUIT_EXECUTION",
    "LIBRARY_MANAGEMENT_SERVICE",
    "HIERARCHICAL_PROJECT_SERVICE",
    "FILEMANAGEMENT_PROJECT_SERVICE",
    "UI_SERVICE",
    "OTOOLKITAPI",
    "OTOOLKIT",
    "OTSYSTEMINFORMATIONTOOL",
    "PASSWORDENCRYPTION",
    "SETPERMISSIONS",
    "CONFIGMONGODBNOAUTH",
    "CONFIGMONGODBWITHAUTH",
    "FILEHEADERUPDATER",
    "ADMINPANEL",
]

# Projects built with a fixed configuration, regardless of what was requested.
BUILD_OVERRIDES: dict[str, tuple[list[str], bool]] = {
    "KEYGENERATOR": (["release"], True),
}
