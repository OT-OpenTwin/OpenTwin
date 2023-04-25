@ECHO OFF

REM The first argument of the shell defines whether a release or debug build shall be performed. BOTH (default) , RELEASE, DEBUG 
REM The second argument of hte shell defines whether a full rebuild or just a build is performed. BUILD, REBUILD (default)
REM Please note that the commands are case-sensitive and that they must not be enclosed in quotes.

REM This script requires the following environment variables to be set:
REM 1. SIM_PLAT_ROOT
REM 2. DEVENV_ROOT_2022

IF "%SIM_PLAT_ROOT%"=="" (
	ECHO Please specify the following environment variables: SIM_PLAT_ROOT
	goto END
)

IF "%DEVENV_ROOT_2022%"=="" (
	ECHO Please specify the following environment variables: DEVENV_ROOT_2022
	goto END
)

REM Setup eviroment
CALL "%SIM_PLAT_ROOT%\MasterBuild\set_env.bat"

ECHO Clean: Authorisation Service
CALL "%SIM_PLAT_ROOT%\Libraries\AuthorisationService\clean.bat"

ECHO Clean: BlockEditorAPI
CALL "%SIM_PLAT_ROOT%\Libraries\BlockEditorAPI\clean.bat"

ECHO Clean: BlockEditor
CALL "%SIM_PLAT_ROOT%\Libraries\BlockEditor\clean.bat"

ECHO Clean: CADModelEntities
CALL "%SIM_PLAT_ROOT%\Libraries\CADModelEntities\clean.bat"

ECHO Clean: Cartesian Mesh Service
CALL "%SIM_PLAT_ROOT%\Libraries\CartesianMeshService\clean.bat"

ECHO Clean: Data Storage Library
CALL "%SIM_PLAT_ROOT%\Libraries\DataStorage\clean.bat"

ECHO Clean: FITTD Service
CALL "%SIM_PLAT_ROOT%\Libraries\FITTDService\clean.bat"

ECHO Clean: Global Directory Service
CALL "%SIM_PLAT_ROOT%\Libraries\GlobalDirectoryService\clean.bat"

ECHO Clean: Global Session Service
CALL "%SIM_PLAT_ROOT%\Libraries\GlobalSessionService\clean.bat"

ECHO Clean: OTGui
CALL "%SIM_PLAT_ROOT%\Libraries\OTGui\clean.bat"

ECHO Clean: Import Parameterized Data Service
CALL "%SIM_PLAT_ROOT%\Libraries\ImportParameterizedData\clean.bat"

ECHO Clean: Kriging Service
CALL "%SIM_PLAT_ROOT%\Libraries\KrigingService\clean.bat"

ECHO Clean: Local Directory Service
CALL "%SIM_PLAT_ROOT%\Libraries\LocalDirectoryService\clean.bat"

ECHO Clean: Logger Gui Service
CALL "%SIM_PLAT_ROOT%\Libraries\LoggerGuiService\clean.bat"

ECHO Clean: Logger Service
CALL "%SIM_PLAT_ROOT%\Libraries\LoggerService\clean.bat"

ECHO Clean: Model Service
CALL "%SIM_PLAT_ROOT%\Libraries\Model\clean.bat"

ECHO Clean: Model Entities Library
CALL "%SIM_PLAT_ROOT%\Libraries\ModelEntities\clean.bat"

ECHO Clean: Modeling Service
CALL "%SIM_PLAT_ROOT%\Libraries\ModelingService\clean.bat"

ECHO Clean: OpenTwin Communication Library
CALL "%SIM_PLAT_ROOT%\Libraries\OpenTwinCommunication\clean.bat"

ECHO Clean: OpenTwin Service Foundation Library
CALL "%SIM_PLAT_ROOT%\Libraries\OpenTwinServiceFoundation\clean.bat"

ECHO Clean: OpenTwin Core Library
CALL "%SIM_PLAT_ROOT%\Libraries\OpenTwinCore\clean.bat"

ECHO Clean: OpenTwin System Library
CALL "%SIM_PLAT_ROOT%\Libraries\OpenTwinSystem\clean.bat"

ECHO Clean: PHREEC Service
CALL "%SIM_PLAT_ROOT%\Libraries\PHREECService\clean.bat"

ECHO Clean: Qwt Wrapper
CALL "%SIM_PLAT_ROOT%\Libraries\QwtWrapper\clean.bat"

ECHO Clean: Relay Service
CALL "%SIM_PLAT_ROOT%\Libraries\RelayService\clean.bat"

ECHO Clean: Rubberband Engine Library: Core
CALL "%SIM_PLAT_ROOT%\Libraries\RubberbandEngineCore\clean.bat"

ECHO Clean: Rubberband Engine Library: OSG Wrapper
CALL "%SIM_PLAT_ROOT%\Libraries\RubberbandEngineOsgWrapper\clean.bat"

ECHO Clean: Service Template
CALL "%SIM_PLAT_ROOT%\Libraries\ServiceTemplate\clean.bat"

ECHO Clean: "Local" Session Service
CALL "%SIM_PLAT_ROOT%\Libraries\SessionService\clean.bat"

ECHO Clean: Tet Mesh Service
CALL "%SIM_PLAT_ROOT%\Libraries\TetMeshService\clean.bat"

ECHO Clean: uiCore
CALL "%SIM_PLAT_ROOT%\Libraries\uiCore\clean.bat"

ECHO Clean: UIPluginAPI Library
CALL "%SIM_PLAT_ROOT%\Libraries\UIPluginAPI\clean.bat"

ECHO Clean: UIPlugin Template
CALL "%SIM_PLAT_ROOT%\Libraries\UIPluginTemplate\clean.bat"

ECHO Clean: UI Service
CALL "%SIM_PLAT_ROOT%\Libraries\uiService\clean.bat"

ECHO Clean: Viewer Library
CALL "%SIM_PLAT_ROOT%\Libraries\Viewer\clean.bat"

ECHO Clean: Visualization Service
CALL "%SIM_PLAT_ROOT%\Libraries\VisualizationService\clean.bat"

:END