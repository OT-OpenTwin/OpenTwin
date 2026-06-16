//@otlicense
#include "Login/LoginData.h"
#include "OTCore/JSON/JSON.h"
namespace ot
{
	namespace Authentication
	{
		std::optional<std::string> loginSSO(LoginData& _loginData);
		std::optional<std::string> refreshToken(LoginData& _loginData);
		bool validateToken(LoginData& _loginData);
		void addAuthenticationData(const LoginData& _loginData, ot::JsonDocument& _doc);
	}
}
