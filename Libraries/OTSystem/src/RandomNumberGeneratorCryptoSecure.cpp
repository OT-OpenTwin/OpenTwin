#include "OTSystem/RandomNumber/RandomNumberGeneratorCryptoSecure.h"
#include "OTSystem/ArchitectureInfo.h"
#include <exception>
#include <cassert>

#ifdef OT_OS_WINDOWS
#define WIN32_NO_STATUS 

// Windows header
#include <Windows.h>
#include <bcrypt.h>

#undef WIN32_NO_STATUS 

#include <ntstatus.h>

uint64_t RandomNumberGeneratorCryptoSecure::createPositiveRandomInt64(uint64_t maxValue)
{
	if (maxValue == 0) 
	{
		assert(false);
		throw std::exception("maxValue must be > 0");
	}

	BCRYPT_ALG_HANDLE randomAlgorithm = nullptr;
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

	NTSTATUS status = BCryptOpenAlgorithmProvider(&randomAlgorithm, algorithmID, provider, flag);
	if (status != STATUS_SUCCESS) {
		assert(false);
		throw std::exception("BCryptOpenAlgorithmProvider failed");
	}



	const ULONG bufferSizeInByte = 8;
	UCHAR randomNumberBuffer[bufferSizeInByte];
	/*
		Use the number in pbBuffer  as additional entropy for the random number. If flag not used, a random number is used for the entropy.
	*/
	//flag += BCRYPT_RNG_USE_ENTROPY_IN_BUFFER;
	/*
		Use system-preffered rng algorithm (randomAlgorithm must be NULL). Only supported at PASSIVE_LEVEL and not supported in Vista.
	*/
	//flag += BCRYPT_USE_SYSTEM_PREFERRED_RNG;
	uint64_t randomNumber = 0;
	if (maxValue == UINT64_MAX) 
	{
		status = BCryptGenRandom(randomAlgorithm, randomNumberBuffer, bufferSizeInByte, flag);
		if (status != STATUS_SUCCESS) {
			BCryptCloseAlgorithmProvider(randomAlgorithm, 0);
			throw std::exception("BCryptGenRandom failed");
		}
		for (ULONG i = 0; i < bufferSizeInByte; i++)
		{
			randomNumber |= static_cast<uint64_t>(randomNumberBuffer[i]) << (i * 8);
		}
	}
	else
	{
		// Here we need to prevent the modulo bias it leads to a slightly non-uniform distribution
		// Calculating a modulo of UINT64_MAX with maxValue will end with maxValue +1 buckets (allowed numbers [0,maxValue]). Each bucket contains the numbers that lead to the bucket. 
		// With a range [0,15] and a max value of 5 we would end up with the buckets [0,5] (6 values in total, so 6 buckets) and modulo-bucket 0: {0,6,12} and modulo-bucket 5 {5, 11}, which is unbalanced. The extra entry in modulo-bucket 0 is called tail.
		uint64_t threshold = maxValue + 1; // Number of possible modulo outputs (bucket number)
		uint64_t max_acceptable = UINT64_MAX - (UINT64_MAX % threshold); // Here we calculate the tail. In the example from before it would be 15-(15 %6) = 12
		// The max acceptable value is a value that we can execute the modulo operation on without creating a bias
		// This procedure keeps the uniform distribution and reduces the rejection probability.
		// P(reject) < threshold / 2^64
		// tail is largest when tail = threshold - 1
		// P(reject)< 2^63 / 2^64 = 50%.
		do {
			status = BCryptGenRandom(randomAlgorithm, randomNumberBuffer, bufferSizeInByte, flag);
			if (status != STATUS_SUCCESS) {
				BCryptCloseAlgorithmProvider(randomAlgorithm, 0);
				throw std::exception("BCryptGenRandom failed");
			}
			randomNumber = 0;
			for (ULONG i = 0; i < bufferSizeInByte; i++) 
			{
				randomNumber |= static_cast<uint64_t>(randomNumberBuffer[i]) << (i * 8);
			}
		} while (randomNumber >= max_acceptable);
		randomNumber %= threshold;
	}
	
	BCryptCloseAlgorithmProvider(randomAlgorithm, 0);

	return randomNumber;
}


#endif // OT_OS_WINDOWS