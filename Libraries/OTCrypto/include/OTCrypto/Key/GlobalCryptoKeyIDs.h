// @otlicense

#pragma once

// std header
#include <string>

namespace ot
{
	class GlobalCryptoKeyIDs
	{
		GlobalCryptoKeyIDs() = delete;
	public:
		static std::string localTokenSignature() { return "opentwin-lcl-tknsig"; };

	};

}