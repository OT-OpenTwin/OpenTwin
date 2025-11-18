#pragma once
#define SECURITY_WIN32 1
#include <string>
#include <Windows.h>
#include <vector>
#include <sspi.h>

std::string encode(std::vector<BYTE> _token);

std::vector<BYTE> decode(const std::string _token);

SECURITY_STATUS acquireCredentialsHandle(CredHandle& _credHandle, TimeStamp& _credTimeStamp, unsigned long _fCredentialUse);