// @otlicense

/*
 * RandomNumberGeneratorCryptoSecure.h
 *
 *  Created on: January 17, 2022
 *      Author: Jan Wagner
 */

//OpenTwin header
#include "OTSystem/ArchitectureInfo.h"
#include "OTRandom/RandomNumberGeneratorCryptoSecure.h"

#ifdef OT_OS_WINDOWS

#define WIN32_NO_STATUS 

// Windows header
#include <Windows.h>
#include <bcrypt.h>

#undef WIN32_NO_STATUS 

#include <ntstatus.h>

unsigned __int64 RandomNumberGeneratorCryptoSecure::GetPositiveRandomInt64(unsigned __int64 maxValue)
{
	BCRYPT_ALG_HANDLE randomAlgorithmus;
	/*--------------------------------------------------------------------------------------------------------------------------------------------------*/
	/*
		Information about the algorithm: https://docs.microsoft.com/en-us/windows/win32/seccng/cng-algorithm-identifiers
	*/
	/*
		Standard: FIPS 186-2, FIPS 140-2, NIST SP 800-90
		-> From Windows Vista SP1/Windows Server 2008 on this algorithm is based on AES counter mode, following the NIST SP 800-90 standard.
		-> Windows Vista: based on hash-based rng following the FIPS 186-2 standard.
		-> Windows 8: Supports FIPS 186-3
	*/
	LPCWSTR algorithmID = L"RNG";
	/*
		Dual elliptic curve random-number generator algorithm, following standard SP800-90.
		-> Windows 8: supports FIPS 186-3 standard.
		-> Deprecated from Windows 10 on. Switch to "RNG"
	*/
	//LPCWSTR algorithmID = L"DUALECRNG";
	/*
		RNG for DSA (Digital Signature Algorithm) following standard FIPS 186-2.
	*/
	//LPCWSTR algorithmID = L"FIPS186DSARNG";
	/*--------------------------------------------------------------------------------------------------------------------------------------------------*/
	/*
		Basic Microsoft CNG provider
	*/
	LPCWSTR provider = L"Microsoft Primitive Provider";

	/*
		TPM (Trusted Platform Module) key storage provider.
	*/
	//LPCWSTR provider = L"Microsoft Platform Crypto Provider";

	/*--------------------------------------------------------------------------------------------------------------------------------------------------*/
	ULONG flag(0);
	/*
		Provider will perform the Hash-Based Messgae Authentication Code (HMAC)algorithm with the specified algorithm.
	*/
	//flag += BCRYPT_ALG_HANDLE_HMAC_FLAG;
	/*
		Loads the provider into the nonpaged memory pool. Only in kernel mode and allows subsequent operations on the provider to be processed at the Dispatch level.
		Handle must not be closed before all dependent objects ahave been freed.
	*/
	//flag += BCRYPT_PROV_DISPATCH;
	/*
		Creates a reusable hashing object. The object can be used for a new hashing operation immediately after calling BCryptFinishHash.
	*/
	//flag += BCRYPT_HASH_REUSABLE_FLAG;

	/*--------------------------------------------------------------------------------------------------------------------------------------------------*/

	NTSTATUS status = BCryptOpenAlgorithmProvider(&randomAlgorithmus, algorithmID, provider, flag);

	if (status == STATUS_SUCCESS)
	{
		ULONG bufferSizeInByte = 8;
		PUCHAR randomNumberBuffer = new UCHAR[bufferSizeInByte];
		ULONG flag(0);
		/*
			Use the number in pbBuffer  as additional entropy for the random number. If flag not used, a random number is used for the entropy.
		*/
		//flag += BCRYPT_RNG_USE_ENTROPY_IN_BUFFER;
		/*
			Use system-preffered rng algorithm (randomAlgorithm must be NULL). Only supported at PASSIVE_LEVEL and not supported in Vista.
		*/
		//flag += BCRYPT_USE_SYSTEM_PREFERRED_RNG;
		unsigned __int64 randomNumber = 0;
		status = BCryptGenRandom(randomAlgorithmus, randomNumberBuffer, bufferSizeInByte, flag);
		if (status == STATUS_SUCCESS)
		{
			for (ULONG i = 0; i < bufferSizeInByte; i++)
			{
				unsigned __int64 partialRandomNumber = abs((__int64)randomNumberBuffer[i]);
				randomNumber += partialRandomNumber << (i * 8);
			}
			randomNumber %= maxValue + 1;
			delete randomNumberBuffer;
			return randomNumber;
		}
		else
		{
			delete randomNumberBuffer;
			throw ("Creating random number for shuffled index vector (solver matrix encryption) failed due to error code: " + status);
		}
	}
	else
	{
		throw ("Initializing random algorithm for shuffled index vector (solver matrix encryption) failed due to error code: " + status);
	}

}

#endif // OT_OS_WINDOWS