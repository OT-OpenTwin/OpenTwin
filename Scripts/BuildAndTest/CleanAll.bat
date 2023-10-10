@ECHO OFF

REM This script requires the following environment variables to be set:
REM 1. OPENTWIN_DEV_ROOT
REM 2. OPENTWIN_THIRDPARTY_ROOT
REM 3. DEVENV_ROOT_2022
IF "%OPENTWIN_DEV_ROOT%" == "" (
	ECHO Please specify the following environment variables: OPENTWIN_DEV_ROOT
	goto PAUSE_END
)

IF "%OPENTWIN_THIRDPARTY_ROOT%" == "" (
	ECHO Please specify the following environment variables: OPENTWIN_THIRDPARTY_ROOT
	goto PAUSE_END
)

IF "%DEVENV_ROOT_2022%" == "" (
	ECHO Please specify the following environment variables: DEVENV_ROOT_2022
	goto PAUSE_END
)

REM Setup eviroment
CALL "%OPENTWIN_DEV_ROOT%\Scripts\SetupEnvironment.bat"

REM Ensure that the script finished successfully
IF NOT "%OPENTWIN_DEV_ENV_DEFINED%" == "1" (
	goto END
)


REM ################################################################################################################################
REM LIBRARIES

ECHO Clean: CADModelEntities
CALL "%OPENTWIN_DEV_ROOT%\Libraries\CADModelEntities\clean.bat"

ECHO Clean: Data Storage Library
CALL "%OPENTWIN_DEV_ROOT%\Libraries\DataStorage\clean.bat"

ECHO Clean: OTGui Library
CALL "%OPENTWIN_DEV_ROOT%\Libraries\OTGui\clean.bat"

ECHO Clean: OTWidgets Library
CALL "%OPENTWIN_DEV_ROOT%\Libraries\OTWidgets\clean.bat"

ECHO Clean: Model Entities Library
CALL "%OPENTWIN_DEV_ROOT%\Libraries\ModelEntities\clean.bat"

ECHO Clean: OpenTwin Communication Library
CALL "%OPENTWIN_DEV_ROOT%\Libraries\OpenTwinCommunication\clean.bat"

ECHO Clean: OpenTwin Service Foundation Library
CALL "%OPENTWIN_DEV_ROOT%\Libraries\OpenTwinServiceFoundation\clean.bat"

ECHO Clean: OpenTwin Core Library
CALL "%OPENTWIN_DEV_ROOT%\Libraries\OpenTwinCore\clean.bat"

ECHO Clean: OpenTwin System Library
CALL "%OPENTWIN_DEV_ROOT%\Libraries\OpenTwinSystem\clean.bat"

ECHO Clean: Qwt Wrapper
CALL "%OPENTWIN_DEV_ROOT%\Libraries\QwtWrapper\clean.bat"

ECHO Clean: OTsci
CALL "%OPENTWIN_DEV_ROOT%\Libraries\OTsci\clean.bat"

ECHO Clean: Rubberband Engine Library: Core
CALL "%OPENTWIN_DEV_ROOT%\Libraries\RubberbandEngineCore\clean.bat"

ECHO Clean: Rubberband Engine Library: OSG Wrapper
CALL "%OPENTWIN_DEV_ROOT%\Libraries\RubberbandEngineOsgWrapper\clean.bat"

ECHO Clean: uiCore
CALL "%OPENTWIN_DEV_ROOT%\Libraries\uiCore\clean.bat"

ECHO Clean: UIPluginAPI Library
CALL "%OPENTWIN_DEV_ROOT%\Libraries\UIPluginAPI\clean.bat"

ECHO Clean: Viewer Library
CALL "%OPENTWIN_DEV_ROOT%\Libraries\Viewer\clean.bat"

REM ################################################################################################################################
REM SERVICES

ECHO Clean: Authorisation Service
CALL "%OPENTWIN_DEV_ROOT%\Services\AuthorisationService\clean.bat"

ECHO Clean: Authorisation Service
CALL "%OPENTWIN_DEV_ROOT%\Services\BlockEditorService\clean.bat"

ECHO Clean: Cartesian Mesh Service
CALL "%OPENTWIN_DEV_ROOT%\Services\CartesianMeshService\clean.bat"

ECHO Clean: Circuit Simulator Service
CALL "%OPENTWIN_DEV_ROOT%\Services\CircuitSimulatorService\clean.bat"

ECHO Clean: FITTD Service
CALL "%OPENTWIN_DEV_ROOT%\Services\FITTDService\clean.bat"

ECHO Clean: GetDP Service
CALL "%OPENTWIN_DEV_ROOT%\Services\GetDPService\clean.bat"

ECHO Clean: Global Directory Service
CALL "%OPENTWIN_DEV_ROOT%\Services\GlobalDirectoryService\clean.bat"

ECHO Clean: Global Session Service
CALL "%OPENTWIN_DEV_ROOT%\Services\GlobalSessionService\clean.bat"

ECHO Clean: Import Parameterized Data Service
CALL "%OPENTWIN_DEV_ROOT%\Services\ImportParameterizedData\clean.bat"

ECHO Clean: Kriging Service
CALL "%OPENTWIN_DEV_ROOT%\Services\KrigingService\clean.bat"

ECHO Clean: Local Directory Service
CALL "%OPENTWIN_DEV_ROOT%\Services\LocalDirectoryService\clean.bat"

ECHO Clean: Logger Service
CALL "%OPENTWIN_DEV_ROOT%\Services\LoggerService\clean.bat"

ECHO Clean: Model Service
CALL "%OPENTWIN_DEV_ROOT%\Services\Model\clean.bat"

ECHO Clean: Modeling Service
CALL "%OPENTWIN_DEV_ROOT%\Services\ModelingService\clean.bat"

ECHO Clean: PHREEC Service
CALL "%OPENTWIN_DEV_ROOT%\Services\PHREECService\clean.bat"

ECHO Clean: Relay Service
CALL "%OPENTWIN_DEV_ROOT%\Services\RelayService\clean.bat"

ECHO Clean: "Local" Session Service
CALL "%OPENTWIN_DEV_ROOT%\Services\SessionService\clean.bat"

ECHO Clean: Tet Mesh Service
CALL "%OPENTWIN_DEV_ROOT%\Services\TetMeshService\clean.bat"

ECHO Clean: UI Service
CALL "%OPENTWIN_DEV_ROOT%\Services\uiService\clean.bat"

ECHO Clean: Visualization Service
CALL "%OPENTWIN_DEV_ROOT%\Services\VisualizationService\clean.bat"

GOTO END

:PAUSE_END
pause
GOTO END

:END
