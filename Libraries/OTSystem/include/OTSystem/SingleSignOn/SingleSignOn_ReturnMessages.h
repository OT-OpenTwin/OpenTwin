#pragma once
#include <string>

namespace ot
{
	namespace SSO_ReturnState
	{
		std::string getTokenExpired() { return "Token expired"; }
		std::string getStateContinue() { return "Continue"; }
		std::string getStateAuthenticated() { return "Authenticated"; }
	}
}