// @otlicense
// File: SignatureProvider.h
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

	class OT_CRYPTO_API SignatureProvider
	{
		OT_DECL_STATICONLY(SignatureProvider)
	public:
		static std::vector<uint8_t> sign(const std::vector<uint8_t>& _privateKey, const std::vector<uint8_t>& _data);
		static bool verify(const std::vector<uint8_t>& _publicKey, const std::vector<uint8_t>& _data, const std::vector<uint8_t>& _signature);
	};

}