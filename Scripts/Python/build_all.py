import sys
from pathlib import Path
from typing import Sequence

sys.path.insert(0, str(Path(__file__).resolve().parent))

from opentwin import build_admin_panel, build_all, build_framework, cli
from opentwin.batch import Step

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

# Steps that are not CMake projects and bring their own build system.
SPECIAL: dict[str, tuple[str, Step]] = {
    "FRAMEWORK": ("Framework", build_framework),
    "ADMINPANEL": ("AdminPanel", build_admin_panel),
}

LOG_DIR = ("Scripts", "BuildAndTest")
SUMMARY = "buildLog_Summary.txt"


def main(argv: Sequence[str]) -> int:
    if len(argv) > 2:
        raise SystemExit("usage: build_all.py [DEBUG|RELEASE|BOTH] [BUILD|REBUILD]")

    env = cli.environment()
    configurations = cli.configurations(cli.argument(argv, 0))
    rebuild = cli.build_type(cli.argument(argv, 1))
    logs = Path(env["OPENTWIN_DEV_ROOT"]).joinpath(*LOG_DIR)
    return build_all(env, BUILD_ORDER, BUILD_OVERRIDES, SPECIAL,
                     configurations, rebuild, logs, SUMMARY)


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
