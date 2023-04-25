@ECHO OFF

REM The first argument of the shell defines whether a release or debug build shall be performed. BOTH (default) , RELEASE, DEBUG 
REM The second argument of hte shell defines whether a full rebuild or just a build is performed. BUILD, REBUILD (default)
REM Please note that the commands are case-sensitive and that they must not be enclosed in quotes.

REM This script requires the following environment variables to be set:
REM 1. SIM_PLAT_ROOT

IF "%SIM_PLAT_ROOT%"=="" (
	ECHO Please specify the following environment variables: SIM_PLAT_ROOT
	goto END
)


ECHO ====================================================================
ECHO Test the libraries 
ECHO ====================================================================

ECHO ===============================================================
ECHO Test Library: DataStorage
ECHO ===============================================================
CALL "%SIM_PLAT_ROOT%\Libraries\DataStorage\test.bat" %1 %2

ECHO ===============================================================
ECHO Test Library: ModelEntities
ECHO ===============================================================
CALL "%SIM_PLAT_ROOT%\Libraries\ModelEntities\test.bat" %1 %2

ECHO ===============================================================
ECHO Test Library: OpenTwinServiceFoundation
ECHO ===============================================================
CALL "%SIM_PLAT_ROOT%\Libraries\OpenTwinServiceFoundation\test.bat" %1 %2

ECHO ===============================================================
ECHO Test Library: OpenTwinCommunication
ECHO ===============================================================
CALL "%SIM_PLAT_ROOT%\Libraries\OpenTwinCommunication\test.bat" %1 %2

ECHO ===============================================================
ECHO Test Library: OpenTwinLogger
ECHO ===============================================================
CALL "%SIM_PLAT_ROOT%\Libraries\OpenTwinLogger\test.bat" %1 %2

ECHO ===============================================================
ECHO Test Library: RelayService
ECHO ===============================================================
CALL "%SIM_PLAT_ROOT%\Libraries\RelayService\test.bat" %1 %2

ECHO ===============================================================
ECHO Test Library: ServiceTemplate
ECHO ===============================================================
CALL "%SIM_PLAT_ROOT%\Libraries\ServiceTemplate\test.bat" %1 %2

ECHO ===============================================================
ECHO Test Library: ServiceTemplate
ECHO ===============================================================
CALL "%SIM_PLAT_ROOT%\Libraries\ServiceTemplate\test.bat" %1 %2

ECHO ===============================================================
ECHO Test Library: PHREECService
ECHO ===============================================================
CALL "%SIM_PLAT_ROOT%\Libraries\PHREECService\test.bat" %1 %2

ECHO ===============================================================
ECHO Test Library: AuthorisationService
ECHO ===============================================================
CALL "%SIM_PLAT_ROOT%\Libraries\AuthorisationService\test.bat" %1 %2

ECHO ===============================================================
ECHO Test Library: CADModelEntities
ECHO ===============================================================
CALL "%SIM_PLAT_ROOT%\Libraries\CADModelEntities\test.bat" %1 %2

ECHO ===============================================================
ECHO Test Library: CartesianMeshService
ECHO ===============================================================
CALL "%SIM_PLAT_ROOT%\Libraries\CartesianMeshService\test.bat" %1 %2

ECHO ===============================================================
ECHO Test Library: TetMeshService
ECHO ===============================================================
CALL "%SIM_PLAT_ROOT%\Libraries\TetMeshService\test.bat" %1 %2

ECHO ===============================================================
ECHO Test Library: FITTDService
ECHO ===============================================================
CALL "%SIM_PLAT_ROOT%\Libraries\FITTDService\test.bat" %1 %2

ECHO ===============================================================
ECHO Test Library: GlobalSessionService
ECHO ===============================================================
CALL "%SIM_PLAT_ROOT%\Libraries\GlobalSessionService\test.bat" %1 %2

ECHO ===============================================================
ECHO Test Library: LoggerService
ECHO ===============================================================
CALL "%SIM_PLAT_ROOT%\Libraries\LoggerService\test.bat" %1 %2

ECHO ===============================================================
ECHO Test Library: Model
ECHO ===============================================================
CALL "%SIM_PLAT_ROOT%\Libraries\Model\test.bat" %1 %2

