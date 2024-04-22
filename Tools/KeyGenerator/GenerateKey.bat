IF NOT EXIST "%OT_ENCRYPTIONKEY_ROOT%\OTEncryptionKey.h" (
	ECHO Updating header file "%OT_ENCRYPTIONKEY_ROOT%\OTEncryptionKey.h"
	
	REM Remove libraries
	DEL "%OPENTWIN_DEV_ROOT%\Tools\KeyGenerator\x64\Release\OTSystem.dll"
	DEL "%OPENTWIN_DEV_ROOT%\Tools\KeyGenerator\x64\Release\OTCore.dll"
	DEL "%OPENTWIN_DEV_ROOT%\Tools\KeyGenerator\x64\Release\OTRandom.dll"
	
	REM Copy libraries
	COPY "%OT_SYSTEM_ROOT%\%OT_DLLR%\OTSystem.dll" "%OPENTWIN_DEV_ROOT%\Tools\KeyGenerator\x64\Release"
	COPY "%OT_CORE_ROOT%\%OT_DLLR%\OTCore.dll" "%OPENTWIN_DEV_ROOT%\Tools\KeyGenerator\x64\Release"
	COPY "%OT_RANDOM_ROOT%\%OT_DLLR%\OTRandom.dll" "%OPENTWIN_DEV_ROOT%\Tools\KeyGenerator\x64\Release"

	CALL "%OPENTWIN_DEV_ROOT%\Tools\KeyGenerator\x64\Release\KeyGenerator.exe" 2048 %1
)