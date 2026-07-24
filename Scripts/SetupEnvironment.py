import importlib.util, os, re
from pathlib import Path

# ===== Services (exposed as OT_<KEY>_SERVICE_ROOT) =====
SERVICES = {
    "AUTHORISATION": "AuthorisationService",
    "MODEL": "Model",
    "GLOBAL_SESSION": "GlobalSessionService",
    "LOCAL_SESSION": "LocalSessionService",
    "GLOBAL_DIRECTORY": "GlobalDirectoryService",
    "LOCAL_DIRECTORY": "LocalDirectoryService",
    "RELAY": "RelayService",
    "LOGGER": "LoggerService",
    "PHREEC": "PHREECService",
    "KRIGING": "KrigingService",
    "MODELING": "ModelingService",
    "VISUALIZATION": "VisualizationService",
    "FITTD": "FITTDService",
    "CARTESIAN_MESH": "CartesianMeshService",
    "TET_MESH": "TetMeshService",
    "IMPORT_PARAMETERIZED_DATA": "ImportParameterizedData",
    "GETDP": "GetDPService",
    "ELMERFEM": "ElmerFEMService",
    "STUDIOSUITE": "StudioSuiteService",
    "LTSPICE": "LTSpiceService",
    "UI": "uiService",
    "PYTHON_EXECUTION": "PythonExecutionService",
    "DATA_PROCESSING": "DataProcessingService",
    "CIRCUIT_SIMULATOR": "CircuitSimulatorService",
    "PYRIT": "PyritService",
    "OPENEMS": "OpenEMSService",
    "LIBRARY_MANAGEMENT": "LibraryManagementService",
    "HIERARCHICAL_PROJECT": "HierarchicalProjectService",
    "FILEMANAGEMENT_PROJECT": "FileManagementProjectService",
}

# ===== Services (explicit names that break the *_SERVICE_ROOT rule) =====
SERVICES_EXTRA = {
    "OT_PYTHON_EXECUTION_ROOT": "PythonExecution",
    "OT_CIRCUIT_EXECUTION_ROOT": "CircuitExecution",
    "OT_DEBUGSERVICE_ROOT": "DebugService",
}

# ===== Libraries =====
LIBRARIES = {
    "OT_CADMODELENTITIES_ROOT": "OTCADEntities",
    "OT_DATASTORAGE_ROOT": "OTDataStorage",
    "OT_MODELENTITIES_ROOT": "OTModelEntities",
    "OT_BLOCKENTITIES_ROOT": "OTBlockEntities",
    "OT_COMMUNICATION_ROOT": "OTCommunication",
    "OT_CORE_ROOT": "OTCore",
    "OT_FOUNDATION_ROOT": "OTServiceFoundation",
    "OT_SYSTEM_ROOT": "OTSystem",
    "OT_MODELAPI_ROOT": "OTModelAPI",
    "OT_GUI_ROOT": "OTGui",
    "OT_GUIAPI_ROOT": "OTGuiAPI",
    "OT_WIDGETS_ROOT": "OTWidgets",
    "OT_FRONTEND_CONNECTOR_API_ROOT": "OTFrontendConnectorAPI",
    "OT_FILE_MANAGER_CONNECTOR_ROOT": "OTFMC",
    "OT_STUDIO_SUITE_CONNECTOR_ROOT": "OTStudioSuiteConnector",
    "OT_LTSPICE_CONNECTOR_ROOT": "OTLTSpiceConnector",
    "OT_RUBBERBANDAPI_ROOT": "OTRubberband",
    "OT_RUBBERBAND_OSG_ROOT": "OTRubberbandOSG",
    "OT_UICORE_ROOT": "uiCore",
    "OT_VIEWER_ROOT": "OTViewer",
    "OT_RESULT_DATA_ACCESS_ROOT": "OTResultDataAccess",
}

# ===== Tools =====
TOOLS = {
    "OT_OTOOLKITAPI_ROOT": "OToolkitAPI",
    "OT_OTOOLKIT_ROOT": "OToolkit",
    "OT_FILEHEADERUPDATER_ROOT": "FileHeaderUpdater",
    "OT_CONFIGMONGODBNOAUTH_ROOT": "ConfigMongoDBNoAuth",
    "OT_CONFIGMONGODBWITHAUTH_ROOT": "ConfigMongoDBWithAuth",
    "OT_ENDPOINTDOCPARSER_ROOT": "EndpointDocParser",
    "OT_GITLINECOUNTER_ROOT": "GitLineCounter",
    "OT_KEYGENERATOR_ROOT": "KeyGenerator",
    "OT_OTSYSTEMINFORMATIONTOOL_ROOT": "OTSystemInformationTool",
    "OT_PASSWORDENCRYPTION_ROOT": "PasswordEncryption",
    "OT_SETPERMISSIONS_ROOT": "SetPermissions",
    "OT_WINDOWSSSO_ROOT": "WindowsSSO",
    "OT_MONGODBUPGRADEMANAGER_ROOT": "MongoDBUpgrader/Upgrader_Exe",
}

# ===== Relative path fragments =====
RELATIVE = {
    "OT_INC": "include",
    "OT_SRC": "src",
    "OT_DLLD": r"x64\Debug",
    "OT_DLLR": r"x64\Release",
    "OT_LIBD": r"x64\Debug",
    "OT_LIBR": r"x64\Release",
    "OT_CDLLD": r"build\windows-debug\Debug",
    "OT_CDLLR": r"build\windows-release\Release",
    "OT_CLIBD": r"build\windows-debug\Debug",
    "OT_CLIBR": r"build\windows-release\Release",
    "OT_TEST": "test",
    "OT_LIBTESTD": r"\x64\DebugTest",
    "OT_LIBTESTR": r"\x64\ReleaseTest",
}

# ===== Composite search paths (expanded against everything above) =====
COMPOSITES = {
    "OT_DEFAULT_SERVICE_INCD": r"%OT_CORE_ROOT%\%OT_INC%;%OT_SYSTEM_ROOT%\%OT_INC%;%OT_GUI_ROOT%\%OT_INC%;%OT_COMMUNICATION_ROOT%\%OT_INC%;%OT_GUIAPI_ROOT%\%OT_INC%;%OT_FOUNDATION_ROOT%\%OT_INC%;%OT_MODELENTITIES_ROOT%\%OT_INC%;%OT_DATASTORAGE_ROOT%\%OT_INC%;%OT_MODELAPI_ROOT%\%OT_INC%;%R_JSON_INCD%;%CURL_INCD%;%MONGO_C_INC%;%MONGO_CXX_INC%;%MONGO_BOOST_INCD%",
    "OT_DEFAULT_SERVICE_INCR": r"%OT_CORE_ROOT%\%OT_INC%;%OT_SYSTEM_ROOT%\%OT_INC%;%OT_GUI_ROOT%\%OT_INC%;%OT_COMMUNICATION_ROOT%\%OT_INC%;%OT_GUIAPI_ROOT%\%OT_INC%;%OT_FOUNDATION_ROOT%\%OT_INC%;%OT_MODELENTITIES_ROOT%\%OT_INC%;%OT_DATASTORAGE_ROOT%\%OT_INC%;%OT_MODELAPI_ROOT%\%OT_INC%;%R_JSON_INCR%;%CURL_INCR%;%MONGO_C_INC%;%MONGO_CXX_INC%;%MONGO_BOOST_INCR%",
    "OT_DEFAULT_SERVICE_LIBPATHD": r"%OT_CORE_ROOT%\%OT_CLIBD%;%OT_SYSTEM_ROOT%\%OT_CLIBD%;%OT_GUI_ROOT%\%OT_CLIBD%;%OT_COMMUNICATION_ROOT%\%OT_CLIBD%;%OT_GUIAPI_ROOT%\%OT_CLIBD%;%OT_FOUNDATION_ROOT%\%OT_CLIBD%;%OT_MODELENTITIES_ROOT%\%OT_CLIBD%;%OT_DATASTORAGE_ROOT%\%OT_CLIBD%;%OT_MODELAPI_ROOT%\%OT_CLIBD%;%CURL_LIBPATHD%;%MONGO_C_LIBPATHD%;%MONGO_CXX_LIBPATHD%",
    "OT_DEFAULT_SERVICE_LIBPATHR": r"%OT_CORE_ROOT%\%OT_CLIBR%;%OT_SYSTEM_ROOT%\%OT_CLIBR%;%OT_GUI_ROOT%\%OT_CLIBR%;%OT_COMMUNICATION_ROOT%\%OT_CLIBR%;%OT_GUIAPI_ROOT%\%OT_CLIBR%;%OT_FOUNDATION_ROOT%\%OT_CLIBR%;%OT_MODELENTITIES_ROOT%\%OT_CLIBR%;%OT_DATASTORAGE_ROOT%\%OT_CLIBR%;%OT_MODELAPI_ROOT%\%OT_CLIBR%;%CURL_LIBPATHR%;%MONGO_C_LIBPATHR%;%MONGO_CXX_LIBPATHR%",
    "OT_DEFAULT_SERVICE_LIBD": r"OTCore.lib;OTSystem.lib;OTGui.lib;OTCommunication.lib;OTGuiAPI.lib;OTServiceFoundation.lib;OTModelEntities.lib;OTDataStorage.lib;OTModelAPI.lib;%CURL_LIBD%;%MONGO_C_LIB%;%MONGO_CXX_LIB%;%MONGO_C_LIBD%",
    "OT_DEFAULT_SERVICE_LIBR": r"OTCore.lib;OTSystem.lib;OTGui.lib;OTCommunication.lib;OTGuiAPI.lib;OTServiceFoundation.lib;OTModelEntities.lib;OTDataStorage.lib;OTModelAPI.lib;%CURL_LIBR%;%MONGO_C_LIB%;%MONGO_CXX_LIB%;%MONGO_C_LIBR%",
    "OT_DEFAULT_SERVICE_DLLD": r"%OT_CORE_ROOT%\%OT_CDLLD%;%OT_SYSTEM_ROOT%\%OT_CDLLD%;%OT_GUI_ROOT%\%OT_CDLLD%;%OT_COMMUNICATION_ROOT%\%OT_CDLLD%;%OT_GUIAPI_ROOT%\%OT_CDLLD%;%OT_FOUNDATION_ROOT%\%OT_CDLLD%;%OT_MODELENTITIES_ROOT%\%OT_CDLLD%;%OT_DATASTORAGE_ROOT%\%OT_CDLLD%;%OT_MODELAPI_ROOT%\%OT_CDLLD%;%CURL_DLLD%;%MONGO_C_DLLD%;%MONGO_CXX_DLLD%;%CURL_DLLD%;%ZLIB_DLLPATHD%;%VTK_DLLD%;%OSG_DLLD%;%QT_DLLD%;%QWT_LIB_DLLD%;%QT_TT_DLLD%",
    "OT_DEFAULT_SERVICE_DLLR": r"%OT_CORE_ROOT%\%OT_CDLLR%;%OT_SYSTEM_ROOT%\%OT_CDLLR%;%OT_GUI_ROOT%\%OT_CDLLR%;%OT_COMMUNICATION_ROOT%\%OT_CDLLR%;%OT_GUIAPI_ROOT%\%OT_CDLLR%;%OT_FOUNDATION_ROOT%\%OT_CDLLR%;%OT_MODELENTITIES_ROOT%\%OT_CDLLR%;%OT_DATASTORAGE_ROOT%\%OT_CDLLR%;%OT_MODELAPI_ROOT%\%OT_CDLLR%;%CURL_DLLR%;%MONGO_C_DLLR%;%MONGO_CXX_DLLR%;%CURL_DLLR%;%ZLIB_DLLPATHR%;%VTK_DLLR%;%OSG_DLLR%;%QT_DLLR%;%QWT_LIB_DLLR%;%QT_TT_DLLR%",
    "OT_ALL_DLLD": r"%OT_RESULT_DATA_ACCESS_ROOT%\%OT_CDLLD%;%OT_CADMODELENTITIES_ROOT%\%OT_CDLLD%;%OT_DATASTORAGE_ROOT%\%OT_CDLLD%;%OT_MODELAPI_ROOT%\%OT_CDLLD%;%OT_MODELENTITIES_ROOT%\%OT_CDLLD%;%OT_BLOCKENTITIES_ROOT%\%OT_CDLLD%;%OT_COMMUNICATION_ROOT%\%OT_CDLLD%;%OT_CORE_ROOT%\%OT_CDLLD%;%OT_GUI_ROOT%\%OT_CDLLD%;%OT_GUIAPI_ROOT%\%OT_CDLLD%;%OT_FOUNDATION_ROOT%\%OT_CDLLD%;%OT_SYSTEM_ROOT%\%OT_CDLLD%;%OT_WIDGETS_ROOT%\%OT_CDLLD%;%OT_RUBBERBANDAPI_ROOT%\%OT_CDLLD%;%OT_RUBBERBAND_OSG_ROOT%\%OT_CDLLD%;%OT_UICORE_ROOT%\%OT_CDLLD%;%OT_VIEWER_ROOT%\%OT_CDLLD%;%MONGO_C_DLLD%;%MONGO_CXX_DLLD%;%CURL_DLLD%;%ZLIB_DLLPATHD%;%VTK_DLLD%;%OSG_DLLD%;%QT_DLLD%;%QWT_LIB_DLLD%;%QT_TT_DLLD%;%OT_STUDIO_SUITE_CONNECTOR_ROOT%\%OT_CDLLD%;%OT_LTSPICE_CONNECTOR_ROOT%\%OT_CDLLD%;%OT_FRONTEND_CONNECTOR_API_ROOT%\%OT_CDLLD%;%OT_FILE_MANAGER_CONNECTOR_ROOT%\%OT_CDLLD%",
    "OT_ALL_DLLR": r"%OT_RESULT_DATA_ACCESS_ROOT%\%OT_CDLLR%;%OT_CADMODELENTITIES_ROOT%\%OT_CDLLR%;%OT_DATASTORAGE_ROOT%\%OT_CDLLR%;%OT_MODELAPI_ROOT%\%OT_CDLLR%;%OT_MODELENTITIES_ROOT%\%OT_CDLLR%;%OT_BLOCKENTITIES_ROOT%\%OT_CDLLR%;%OT_COMMUNICATION_ROOT%\%OT_CDLLR%;%OT_CORE_ROOT%\%OT_CDLLR%;%OT_GUI_ROOT%\%OT_CDLLR%;%OT_GUIAPI_ROOT%\%OT_CDLLR%;%OT_FOUNDATION_ROOT%\%OT_CDLLR%;%OT_SYSTEM_ROOT%\%OT_CDLLR%;%OT_WIDGETS_ROOT%\%OT_CDLLR%;%OT_RUBBERBANDAPI_ROOT%\%OT_CDLLR%;%OT_RUBBERBAND_OSG_ROOT%\%OT_CDLLR%;%OT_UICORE_ROOT%\%OT_CDLLR%;%OT_VIEWER_ROOT%\%OT_CDLLR%;%MONGO_C_DLLR%;%MONGO_CXX_DLLR%;%CURL_DLLR%;%ZLIB_DLLPATHR%;%VTK_DLLR%;%OSG_DLLR%;%QT_DLLR%;%QWT_LIB_DLLR%;%QT_TT_DLLR%;%OT_STUDIO_SUITE_CONNECTOR_ROOT%\%OT_CDLLR%;%OT_LTSPICE_CONNECTOR_ROOT%\%OT_CDLLR%;%OT_FRONTEND_CONNECTOR_API_ROOT%\%OT_CDLLR%;%OT_FILE_MANAGER_CONNECTOR_ROOT%\%OT_CDLLR%",
}

REQUIRED = ["OPENTWIN_DEV_ROOT", "OPENTWIN_THIRDPARTY_ROOT", "DEVENV_ROOT_2022"]

_VAR = re.compile(r"%([^%]+)%")


# ===== Helpers =====
def check_required():
    missing = [v for v in REQUIRED if not os.environ.get(v)]
    if missing:
        raise SystemExit("Please specify the following environment variables: " + ", ".join(missing))


def _get(env, name):
    if name in env:
        return env[name]
    low = name.lower()
    return next((v for k, v in env.items() if k.lower() == low), "")


def _expand(env, value):
    return _VAR.sub(lambda m: _get(env, m.group(1)), value)


def _load(name, path):
    spec = importlib.util.spec_from_file_location(name, path)
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


# ===== Build =====
def build_env(base=None):
    check_required()
    env = os.environ.copy()
    dev = Path(base or env["OPENTWIN_DEV_ROOT"])
    third = Path(env["OPENTWIN_THIRDPARTY_ROOT"])

    # ThirdParty environment
    _load("ot_thirdparty_env", str(third / "SetupEnvironment.py")).apply(env)

    # OpenTwin roots and relative fragments
    env["OT_ENCRYPTIONKEY_ROOT"] = str(dev / "Certificates" / "Generated")
    env.update(RELATIVE)
    for key, folder in SERVICES.items():
        env[f"OT_{key}_SERVICE_ROOT"] = str(dev / "Services" / folder)
    for group, sub in ((SERVICES_EXTRA, "Services"), (LIBRARIES, "Libraries"), (TOOLS, "Tools")):
        for var, folder in group.items():
            env[var] = str(dev / sub / folder)
    env["OT_DOCUMENTATION_ROOT"] = str(dev / "Documentation" / "Developer")

    # Certificates
    certs = Path(env.get("OPEN_TWIN_CERTS_PATH") or dev / "Certificates" / "Generated")
    env["OPEN_TWIN_CA_CERT"] = str(certs / "ca.pem")
    env["OPEN_TWIN_CERT_KEY"] = str(certs / "certificateKeyFile.pem")
    env["OPEN_TWIN_SERVER_CERT"] = str(certs / "server.pem")
    env["OPEN_TWIN_SERVER_CERT_KEY"] = str(certs / "server-key.pem")

    # Composite search paths
    for name, template in COMPOSITES.items():
        env[name] = _expand(env, template)

    # Service arguments (ports, addresses, logging)
    _load("ot_service_args", str(dev / "Scripts" / "Launcher" / "OpenTwin_set_up_service_args.py")).apply(env)

    # Finalize
    env["OT_CMAKE_DIR"] = str(dev / "Scripts" / "CMake")
    env["PATH"] = _expand(env, r"%OPENTWIN_DEV_ROOT%\Deployment;%PATH%")
    env["PATH"] = _expand(env, r"%ProgramFiles%\MiKTeX\miktex\bin\x64;%PATH%")
    env["OPENTWIN_DEV_ENV_DEFINED"] = "1"
    print("OpenTwin Developer environment was set up successfully.")
    return env
