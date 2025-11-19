#pragma once
#define SECURITY_WIN32 1
#include <string>
#include <Windows.h>
#include <vector>
#include <sspi.h>

SECURITY_STATUS acquireCredentialsHandle(CredHandle& _credHandle, TimeStamp& _credTimeStamp, unsigned long _fCredentialUse);