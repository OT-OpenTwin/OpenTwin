@ECHO OFF

REM The first argument of the shell defines whether a release or debug build shall be performed. BOTH (default) , RELEASE, DEBUG 
REM The second argument of hte shell defines whether a full rebuild or just a build is performed. BUILD, REBUILD (default)
REM Please note that the commands are case-sensitive and that they must not be enclosed in quotes.

REM This script requires the following environment variables to be set:
REM 1. OPENTWIN_DEV_ROOT
REM 2. DEVENV_ROOT_2022

IF "%OPENTWIN_DEV_ROOT%"=="" (
	ECHO Please specify the following environment variables: OPENTWIN_DEV_ROOT
	goto END
)

IF "%DEVENV_ROOT_2022%"=="" (
	ECHO Please specify the following environment variables: DEVENV_ROOT_2022
	goto END
)

REM Setup eviroment
CALL "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\SetupEnvironment.bat"

ECHO Clean: Authorisation Service
CALL "%OPENTWIN_DEV_ROOT%\Libraries\AuthorisationService\clean.bat"

ECHO Clean: BlockEditorAPI
CALL "%OPENTWIN_DEV_ROOT%\Libraries\BlockEditorAPI\clean.bat"

ECHO Clean: BlockEditor
CALL "%OPENTWIN_DEV_ROOT%\Libraries\BlockEditor\clean.bat"

ECHO Clean: CADModelEntities
CALL "%OPENTWIN_DEV_ROOT%\Libraries\CADModelEntities\clean.bat"

ECHO Clean: Cartesian Mesh Service
CALL "%OPENTWIN_DEV_ROOT%\Libraries\CartesianMeshService\clean.bat"

ECHO Clean: Data Storage Library
CALL "%OPENTWIN_DEV_ROOT%\Libraries\DataStorage\clean.bat"

ECHO Clean: FITTD Service
CALL "%OPENTWIN_DEV_ROOT%\Libraries\FITTDService\clean.bat"

ECHO Clean: Global Directory Service
CALL "%OPENTWIN_DEV_ROOT%\Libraries\GlobalDirectoryService\clean.bat"

ECHO Clean: Global Session Service
CALL "%OPENTWIN_DEV_ROOT%\Libraries\GlobalSessionService\clean.bat"

ECHO Clean: OTGui
CALL "%OPENTWIN_DEV_ROOT%\Libraries\OTGui\clean.bat"

ECHO Clean: Import Parameterized Data Service
CALL "%OPENTWIN_DEV_ROOT%\Libraries\ImportParameterizedData\clean.bat"

ECHO Clean: Kriging Service
CALL "%OPENTWIN_DEV_ROOT%\Libraries\KrigingService\clean.bat"

ECHO Clean: Local Directory Service
CALL "%OPENTWIN_DEV_ROOT%\Libraries\LocalDirectoryService\clean.bat"

ECHO Clean: Logger Gui Service
CALL "%OPENTWIN_DEV_ROOT%\Libraries\LoggerGuiService\clean.bat"

ECHO Clean: Logger Service
CALL "%OPENTWIN_DEV_ROOT%\Libraries\LoggerService\clean.bat"

ECHO Clean: Model Service
CALL "%OPENTWIN_DEV_ROOT%\Libraries\Model\clean.bat"

ECHO Clean: Model Entities Library
CALL "%OPENTWIN_DEV_ROOT%\Libraries\ModelEntities\clean.bat"

ECHO Clean: Modeling Service
CALL "%OPENTWIN_DEV_ROOT%\Libraries\ModelingService\clean.bat"

ECHO Clean: OpenTwin Communication Library
CALL "%OPENTWIN_DEV_ROOT%\Libraries\OpenTwinCommunication\clean.bat"

ECHO Clean: OpenTwin Service Foundation Library
CALL "%OPENTWIN_DEV_ROOT%\Libraries\OpenTwinServiceFoundation\clean.bat"

ECHO Clean: OpenTwin Core Library
CALL "%OPENTWIN_DEV_ROOT%\Libraries\OpenTwinCore\clean.bat"

ECHO Clean: OpenTwin System Library
CALL "%OPENTWIN_DEV_ROOT%\Libraries\OpenTwinSystem\clean.bat"

ECHO Clean: PHREEC Service
CALL "%OPENTWIN_DEV_ROOT%\Libraries\PHREECService\clean.bat"

ECHO Clean: Qwt Wrapper
CALL "%OPENTWIN_DEV_ROOT%\Libraries\QwtWrapper\clean.bat"

ECHO Clean: Relay Service
CALL "%OPENTWIN_DEV_ROOT%\Libraries\RelayService\clean.bat"

ECHO Clean: Rubberband Engine Library: Core
CALL "%OPENTWIN_DEV_ROOT%\Libraries\RubberbandEngineCore\clean.bat"

ECHO Clean: Rubberband Engine Library: OSG Wrapper
CALL "%OPENTWIN_DEV_ROOT%\Libraries\RubberbandEngineOsgWrapper\clean.bat"

ECHO Clean: Service Template
CALL "%OPENTWIN_DEV_ROOT%\Libraries\ServiceTemplate\clean.bat"

ECHO Clean: "Local" Session Service
CALL "%OPENTWIN_DEV_ROOT%\Libraries\SessionService\clean.bat"

ECHO Clean: Tet Mesh Service
CALL "%OPENTWIN_DEV_ROOT%\Libraries\TetMeshService\clean.bat"

ECHO Clean: uiCore
CALL "%OPENTWIN_DEV_ROOT%\Libraries\uiCore\clean.bat"

ECHO Clean: UIPluginAPI Library
CALL "%OPENTWIN_DEV_ROOT%\Libraries\UIPluginAPI\clean.bat"

ECHO Clean: UIPlugin Template
CALL "%OPENTWIN_DEV_ROOT%\Libraries\UIPluginTemplate\clean.bat"

ECHO Clean: UI Service
CALL "%OPENTWIN_DEV_ROOT%\Libraries\uiService\clean.bat"

ECHO Clean: Viewer Library
CALL "%OPENTWIN_DEV_ROOT%\Libraries\Viewer\clean.bat"

ECHO Clean: Visualization Service
CALL "%OPENTWIN_DEV_ROOT%\Libraries\VisualizationService\clean.bat"

:END