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

