@ECHO OFF

REM The first argument of the shell defines whether a release or debug build shall be performed. BOTH (default) , RELEASE, DEBUG 
REM The second argument of hte shell defines whether a full rebuild or just a build is performed. BUILD, REBUILD (default)
REM Please note that the commands are case-sensitive and that they must not be enclosed in quotes.

REM This script requires the following environment variables to be set:
REM 1. OPENTWIN_DEV_ROOT

CALL "%OPENTWIN_DEV_ROOT%\Scripts\SetupEnvironment.bat"

IF "%OPENTWIN_DEV_ROOT%"=="" (
	ECHO Please specify the following environment variables: OPENTWIN_DEV_ROOT
	goto END
)
SET testBat=\test.bat

ECHO ====================================================================
ECHO Test the services 
ECHO ====================================================================

ECHO ===============================================================
ECHO Test Service: Authorisation
ECHO ===============================================================
CALL "%OT_AUTHORISATION_SERVICE_ROOT%%testBat%" %1 %2

ECHO ===============================================================
ECHO Test Service: Model
ECHO ===============================================================
CALL "%OT_MODEL_SERVICE_ROOT%%testBat%" %1 %2

ECHO ===============================================================
ECHO Test Service: Global Session
ECHO ===============================================================
CALL "%OT_GLOBAL_SESSION_SERVICE_ROOT%%testBat%" %1 %2

ECHO ===============================================================
ECHO Test Service: Local Session
ECHO ===============================================================
CALL "%OT_LOCAL_SESSION_SERVICE_ROOT%%testBat%" %1 %2

ECHO ===============================================================
ECHO Test Service: Global Directory
ECHO ===============================================================
CALL "%OT_GLOBAL_DIRECTORY_SERVICE_ROOT%%testBat%" %1 %2

ECHO ===============================================================
ECHO Test Service: Local Directory
ECHO ===============================================================
CALL "%OT_LOCAL_DIRECTORY_SERVICE_ROOT%%testBat%" %1 %2

ECHO ===============================================================
ECHO Test Service: Relay
ECHO ===============================================================
CALL "%OT_RELAY_SERVICE_ROOT%%testBat%" %1 %2

ECHO ===============================================================
ECHO Test Service: Logger
ECHO ===============================================================
CALL "%OT_LOGGER_SERVICE_ROOT%%testBat%" %1 %2

ECHO ===============================================================
ECHO Test Service: PHREEC
ECHO ===============================================================
CALL "%OT_PHREEC_SERVICE_ROOT%%testBat%" %1 %2

ECHO ===============================================================
ECHO Test Service: KRIGING
ECHO ===============================================================
CALL "%OT_KRIGING_SERVICE_ROOT%%testBat%" %1 %2

ECHO ===============================================================
ECHO Test Service: Modeling
ECHO ===============================================================
CALL "%OT_MODELING_SERVICE_ROOT%%testBat% %1 %2

ECHO ===============================================================
ECHO Test Service: Visualization
ECHO ===============================================================
CALL "%OT_VISUALIZATION_SERVICE_ROOT%%testBat%" %1 %2

ECHO ===============================================================
ECHO Test Service: FITTD
ECHO ===============================================================
CALL "%OT_FITTD_SERVICE_ROOT%%testBat% %1 %2

ECHO ===============================================================
ECHO Test Service: Cartesian Mesh
ECHO ===============================================================
CALL "%OT_CARTESIAN_MESH_SERVICE_ROOT%%testBat%" %1 %2

ECHO ===============================================================
ECHO Test Service: Tet Mesh
ECHO ===============================================================
CALL "%OT_TET_MESH_SERVICE_ROOT%%testBat%" %1 %2

ECHO ===============================================================
ECHO Test Service: Import Parameterized Data
ECHO ===============================================================
CALL "%OT_IMPORT_PARAMETERIZED_DATA_SERVICE_ROOT%%testBat%" %1 %2

ECHO ===============================================================
ECHO Test Service: GetDP
ECHO ===============================================================
CALL "%OT_GETDP_SERVICE_ROOT%%testBat%" %1 %2

ECHO ===============================================================
ECHO Test Service: ElmerFEM
ECHO ===============================================================
CALL "%OT_ELMERFEM_SERVICE_ROOT%%testBat%" %1 %2

ECHO ===============================================================
ECHO Test Service: StudioSuite
ECHO ===============================================================
CALL "%OT_STUDIOSUITE_SERVICE_ROOT%%testBat%" %1 %2

ECHO ===============================================================
ECHO Test Service: Block Editor
ECHO ===============================================================
CALL "%OT_BLOCKEDITORSERVICE_ROOT%%testBat%" %1 %2

ECHO ===============================================================
ECHO Test Service: UI
ECHO ===============================================================
CALL "%OT_UI_SERVICE_ROOT%%testBat%" %1 %2

ECHO ===============================================================
ECHO Test Service: Circuit Simulator
ECHO ===============================================================
CALL "%OT_CIRCUIT_SIMULATOR_SERVICE_ROOT%%testBat%" %1 %2



ECHO ====================================================================
ECHO Test the libraries
ECHO ====================================================================

ECHO ===============================================================
ECHO Test Datastorage: DataStorage
ECHO ===============================================================
CALL "%OT_DATASTORAGE_ROOT%%testBat%" %1 %2

ECHO ===============================================================
ECHO Test Library: ModelEntities
ECHO ===============================================================
CALL "%OT_MODELENTITIES_ROOT%%testBat%" %1 %2

ECHO ===============================================================
ECHO Test Library: OpenTwinServiceFoundation
ECHO ===============================================================
CALL "%OT_FOUNDATION_ROOT%%testBat%" %1 %2

ECHO ===============================================================
ECHO Test Library: OpenTwinCommunication
ECHO ===============================================================
CALL "%OT_COMMUNICATION_ROOT%%testBat%" %1 %2

ECHO ===============================================================
ECHO Test Library: CADModelEntities
ECHO ===============================================================
CALL "%OT_CADMODELENTITIES_ROOT%%testBat%" %1 %2

ECHO ===============================================================
ECHO Test Library: Core
ECHO ===============================================================
CALL "%OT_CORE_ROOT%%testBat%" %1 %2

ECHO ===============================================================
ECHO Test Library: OTRandom
ECHO ===============================================================
CALL "%OT_RANDOM_ROOT%%testBat%" %1 %2

ECHO ===============================================================
ECHO Test Library: System
ECHO ===============================================================
CALL "%OT_SYSTEM_ROOT%%testBat%" %1 %2

ECHO ===============================================================
ECHO Test Library: OTGui
ECHO ===============================================================
CALL "%OT_GUI_ROOT%%testBat%" %1 %2

ECHO ===============================================================
ECHO Test Library: OTGuiAPI
ECHO ===============================================================
CALL "%OT_GUIAPI_ROOT%%testBat%" %1 %2

ECHO ===============================================================
ECHO Test Library: OTWidgets
ECHO ===============================================================
CALL "%OT_WIDGETS_ROOT%%testBat%" %1 %2

ECHO ===============================================================
ECHO Test Library: QWTWrapper
ECHO ===============================================================
CALL "%OT_QWTWRAPPER_ROOT%%testBat%" %1 %2

ECHO ===============================================================
ECHO Test Library: RubberbandAPI
ECHO ===============================================================
CALL "%OT_RUBBERBANDAPI_ROOT%%testBat%" %1 %2

ECHO ===============================================================
ECHO Test Library: Rubberband
ECHO ===============================================================
CALL "%OT_RUBBERBAND_ROOT%%testBat%" %1 %2

ECHO ===============================================================
ECHO Test Library: UICore
ECHO ===============================================================
CALL "%OT_UICORE_ROOT%%testBat%" %1 %2

ECHO ===============================================================
ECHO Test Library: Viewer
ECHO ===============================================================
CALL "%OT_VIEWER_ROOT%%testBat%" %1 %2

