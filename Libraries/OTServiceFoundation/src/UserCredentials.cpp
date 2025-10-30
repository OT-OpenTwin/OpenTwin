// @otlicense
// File: UserCredentials.cpp
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

// Open Twin header
#include "OTServiceFoundation/UserCredentials.h"		// Corresponding header
#include "OTServiceFoundation/Encryption.h"

std::string ot::UserCredentials::encryptString(const std::string& _raw) {
	return ot::Encryption::encryptString(_raw);
}

std::string ot::UserCredentials::decryptString(const std::string& _encrypted) {
	return ot::Encryption::decryptString(_encrypted);
}