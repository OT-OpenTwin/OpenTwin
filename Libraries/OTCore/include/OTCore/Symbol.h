// @otlicense
// File: Symbol.h
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
#include "OTCore/CoreTypes.h"

// std header
#include <string_view>

namespace ot {

	//! @brief The Symbol class returns UTF-8 encoded string views of common symbols regardless of the source file encoding.
	class Symbol
	{
		OT_DECL_STATICONLY(Symbol)
	public:

		// Lowercase greek letters

		static constexpr std::string_view alpha()   noexcept { return "\xCE\xB1"; };
		static constexpr std::string_view beta()    noexcept { return "\xCE\xB2"; };
		static constexpr std::string_view gamma()   noexcept { return "\xCE\xB3"; };
		static constexpr std::string_view delta()   noexcept { return "\xCE\xB4"; };
		static constexpr std::string_view epsilon() noexcept { return "\xCE\xB5"; };
		static constexpr std::string_view zeta()    noexcept { return "\xCE\xB6"; };
		static constexpr std::string_view eta()     noexcept { return "\xCE\xB7"; };
		static constexpr std::string_view theta()   noexcept { return "\xCE\xB8"; };
		static constexpr std::string_view lambda()  noexcept { return "\xCE\xBB"; };
		static constexpr std::string_view mu()      noexcept { return "\xCE\xBC"; };
		static constexpr std::string_view nu()      noexcept { return "\xCE\xBD"; };
		static constexpr std::string_view pi()      noexcept { return "\xCF\x80"; };
		static constexpr std::string_view rho()     noexcept { return "\xCF\x81"; };
		static constexpr std::string_view sigma()   noexcept { return "\xCF\x83"; };
		static constexpr std::string_view tau()     noexcept { return "\xCF\x84"; };
		static constexpr std::string_view phi()     noexcept { return "\xCF\x86"; };
		static constexpr std::string_view chi()     noexcept { return "\xCF\x87"; };
		static constexpr std::string_view psi()     noexcept { return "\xCF\x88"; };
		static constexpr std::string_view omega()   noexcept { return "\xCF\x89"; };

		// Uppercase greek letters

		static constexpr std::string_view Gamma()   noexcept { return "\xCE\x93"; };
		static constexpr std::string_view Delta()   noexcept { return "\xCE\x94"; };
		static constexpr std::string_view Theta()   noexcept { return "\xCE\x98"; };
		static constexpr std::string_view Lambda()  noexcept { return "\xCE\x9B"; };
		static constexpr std::string_view Pi()      noexcept { return "\xCE\xA0"; };
		static constexpr std::string_view Sigma()   noexcept { return "\xCE\xA3"; };
		static constexpr std::string_view Phi()     noexcept { return "\xCE\xA6"; };
		static constexpr std::string_view Psi()     noexcept { return "\xCE\xA8"; };
		static constexpr std::string_view Omega()   noexcept { return "\xCE\xA9"; };

		// Mathematical Operators

		static constexpr std::string_view plusMinus() noexcept { return "\xC2\xB1"; };
		static constexpr std::string_view times()     noexcept { return "\xC3\x97"; };
		static constexpr std::string_view divide()    noexcept { return "\xC3\xB7"; };
		static constexpr std::string_view dot()       noexcept { return "\xC2\xB7"; };
		static constexpr std::string_view sqrt()      noexcept { return "\xE2\x88\x9A"; };

		// Comparison Operators

		static constexpr std::string_view lessEqual()     noexcept { return "\xE2\x89\xA4"; };
		static constexpr std::string_view greaterEqual()  noexcept { return "\xE2\x89\xA5"; };
		static constexpr std::string_view notEqual()      noexcept { return "\xE2\x89\xA0"; };
		static constexpr std::string_view approx()        noexcept { return "\xE2\x89\x88"; };
		static constexpr std::string_view equivalent()    noexcept { return "\xE2\x89\xA1"; };

		// Calculus / Analysis

		static constexpr std::string_view integral()      noexcept { return "\xE2\x88\xAB"; };
		static constexpr std::string_view partial()       noexcept { return "\xE2\x88\x82"; };
		static constexpr std::string_view nabla()         noexcept { return "\xE2\x88\x87"; };
		static constexpr std::string_view infinity()      noexcept { return "\xE2\x88\x9E"; };
		static constexpr std::string_view sum()           noexcept { return "\xE2\x88\x91"; };
		static constexpr std::string_view product()       noexcept { return "\xE2\x88\x8F"; };

		// Set Theory

		static constexpr std::string_view elementOf()     noexcept { return "\xE2\x88\x88"; };
		static constexpr std::string_view notElementOf()  noexcept { return "\xE2\x88\x89"; };
		static constexpr std::string_view subset()        noexcept { return "\xE2\x8A\x82"; };
		static constexpr std::string_view subsetEqual()   noexcept { return "\xE2\x8A\x86"; };
		static constexpr std::string_view unionSet()      noexcept { return "\xE2\x88\xAA"; };
		static constexpr std::string_view intersection()  noexcept { return "\xE2\x88\xA9"; };

		// Logic

		static constexpr std::string_view logicalAnd() noexcept { return "\xE2\x88\xA7"; };
		static constexpr std::string_view logicalOr()  noexcept { return "\xE2\x88\xA8"; };
		static constexpr std::string_view implies()    noexcept { return "\xE2\x87\x92"; };
		static constexpr std::string_view iff()        noexcept { return "\xE2\x87\x94"; };

		// Units / Physics

		static constexpr std::string_view degree() noexcept { return "\xC2\xB0"; };
		static constexpr std::string_view micro()  noexcept { return "\xC2\xB5"; };
		static constexpr std::string_view ohm()    noexcept { return "\xCE\xA9"; };
	};

}