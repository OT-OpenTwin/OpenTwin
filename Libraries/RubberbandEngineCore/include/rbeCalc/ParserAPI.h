// @otlicense
// File: ParserAPI.h
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

// RBE header
#include <rbeCore/dataTypes.h>

// C++ header
#include <string>

namespace rbeCore {
	class RubberbandEngine;
	class AbstractPoint;
	class Step;
}

namespace rbeCalc {

	class AbstractCalculationItem;

	namespace ParserAPI {

		RBE_API_EXPORT rbeCalc::AbstractCalculationItem * parseFormula(rbeCore::RubberbandEngine * _engine, rbeCore::Step * _step, const std::string& _string);
	
		RBE_API_EXPORT rbeCore::AbstractPoint * parsePoint(rbeCore::RubberbandEngine * _engine, rbeCore::Step * _step, const std::string& _str, rbeCore::eAxisDistance& _distance);

	}
}