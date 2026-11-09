// @otlicense
// File: CryptoKeyPair.h
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
#include "OTCrypto/CryptoTypes.h"

namespace ot
{
	class OT_CRYPTO_API CryptoKeyPair
	{
		OT_DECL_NOCOPY(CryptoKeyPair)
		OT_DECL_DEFMOVE(CryptoKeyPair)
	public:
		explicit CryptoKeyPair() = default;
		explicit CryptoKeyPair(std::vector<uint8_t>&& _publicKey, std::vector<uint8_t>&& _privateKey);
		~CryptoKeyPair() = default;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setPublicKey(std::vector<uint8_t>&& _publicKey) { m_publicKey = std::move(_publicKey); };
		const std::vector<uint8_t>& getPublicKey() const { return m_publicKey; };

		void setPrivateKey(std::vector<uint8_t>&& _privateKey) { m_privateKey = std::move(_privateKey); };
		const std::vector<uint8_t>& getPrivateKey() const { return m_privateKey; };

	private:
		std::vector<uint8_t> m_publicKey;
		std::vector<uint8_t> m_privateKey;

	};

}