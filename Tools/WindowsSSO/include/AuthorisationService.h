// @otlicense
// File: AuthorisationService.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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
#include "AuthorisationServiceAPI.h"
class AuthorisationService :public AuthorisationServiceAPI
{
public:
    AuthorisationService();
    std::string generateToken2(const std::string& _token1) override;
    void authorizeClient(const std::string& _token3) override;

private:
    CredHandle m_credHandle;
    TimeStamp m_credTimeStamp;

    // On the first call to AcceptSecurityContext(CredSSP), this pointer receives the new context handle.
    // On subsequent calls, m_partialContext can be the same as this handle.
    CtxtHandle m_currentContext;
    // On the first call to AcceptSecurityContext (CredSSP), this pointer is NULL. 
    // On subsequent calls, phContext specifies the partially formed context returned in the phNewContext parameter by the first call.
    CtxtHandle* m_partialContext = nullptr;

    void readUserName(CtxtHandle& _completedContext);
    void readUserGroups(CtxtHandle _completedContext);
};

