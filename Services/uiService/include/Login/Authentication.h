//@otlicense
#include "Login/LoginData.h"
#include "OTCore/JSON/JSON.h"
namespace ot
{
	namespace Authentication
	{
		std::optional<std::string> loginSSO(LoginData& _loginData, std::string& _customTitle, std::string& _customMsg);
		std::optional<std::string> refreshToken(LoginData& _loginData);
		bool validateAndRefreshToken(LoginData& _loginData);
		void addAuthenticationData(const LoginData& _loginData, ot::JsonDocument& _doc);
	}
}
