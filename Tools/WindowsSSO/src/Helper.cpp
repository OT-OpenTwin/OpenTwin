// @otlicense
// File: Helper.cpp
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

#include "Helper.h"

SECURITY_STATUS acquireCredentialsHandle(CredHandle& _credHandle, TimeStamp& _credTimeStamp, unsigned long _fCredentialUse)
{
    LPWSTR pPrincipal = NULL; //An entity recognized by the security system. Principals can include human users as well as autonomous processes.
    LPWSTR pPackage = const_cast<LPWSTR>(L"Negotiate"); //The software implementation of a security protocol. Options: Kerberos, NTLM, Schannel and CredSSP
    unsigned long fCredentialUse = _fCredentialUse; //A flag that indicates how these credentials will be used. SECPKG_CRED_OUTBOUND = Allow a local client credential to prepare an outgoing token. 
    void* pvLogonId = NULL; //This parameter is provided for file-system processes such as network redirectors.
    void* pAuthData = NULL; //Specifies authentication data for both Schannel and Negotiate packages
    SEC_GET_KEY_FN  pGetKeyFn = NULL;  //Reserved. This parameter is not used and should be set to NULL.
    void* pvGetKeyArgument = NULL; //Reserved. This parameter must be set to NULL.

    SECURITY_STATUS securityStatus = AcquireCredentialsHandleW(
        pPrincipal,
        pPackage,
        fCredentialUse,
        pvLogonId,
        pAuthData,
        pGetKeyFn,
        pvGetKeyArgument,
        &_credHandle,
        &_credTimeStamp);

    return securityStatus;
}

