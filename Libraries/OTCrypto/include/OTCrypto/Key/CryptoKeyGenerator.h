// @otlicense
// File: CryptoKeyGenerator.h
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

// OpenTwin header
#include "OTCrypto/Key/CryptoKeyPair.h"

namespace ot
{

	class OT_CRYPTO_API CryptoKeyGenerator
	{
		OT_DECL_STATICONLY(CryptoKeyGenerator)
	public:
		//! @brief Generates a new asymmetric key pair.
		//! @return A CryptoKeyPair object containing the generated public and private keys.
		//! @throws ot::RuntimeException if key generation fails.
		OT_DECL_NODISCARD static CryptoKeyPair generateAsymmetricKeyPair();
	};

}