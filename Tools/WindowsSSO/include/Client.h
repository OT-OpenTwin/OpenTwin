#pragma once
#define SECURITY_WIN32 1
#include <windows.h>
#include <sspi.h>
#include <strsafe.h>
#include <iostream>
#include <vector>
#include <string>
#include <winhttp.h>
#pragma comment(lib, "secur32.lib")
#pragma comment(lib, "winhttp.lib")
#include <cassert>
#include "Helper.h"

class Client
{
public:
    Client();


    std::vector<BYTE> generateClientToken(const std::vector<BYTE>& _inputToken, bool _firstCall);

private:
    CredHandle m_credHandle;
    TimeStamp m_credTimeStamp;
    // On the first call to AcceptSecurityContext(CredSSP), this pointer receives the new context handle.
    // On subsequent calls, m_partialContext can be the same as this handle.
    CtxtHandle m_currentContext;
    // On the first call to AcceptSecurityContext (CredSSP), this pointer is NULL. 
    // On subsequent calls, phContext specifies the partially formed context returned in the phNewContext parameter by the first call.
    CtxtHandle* m_partialContext = nullptr;
};


