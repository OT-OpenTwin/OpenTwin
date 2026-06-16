#include "Login/Authentication.h"
#include "AppBase.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Msg.h"
#include "OTCore/ReturnMessage.h"
#include "OTSystem/SingleSignOn/SingleSignOn_ReturnMessages.h"
#include "OTSystem/SingleSignOn/SingleSignOn_Client.h"
#include <functional>


std::optional<std::string> ssoSequence(LoginData& _loginData, const std::function<void(LoginData&, ot::JsonDocument&)>& _processSuccessfulSSO, const std::string& _authorisationServiceEndpoint)
{
	try
	{
		ot::SingleSignOn_Client client;
		const std::string& authorisationURL = _loginData.getAuthorizationUrl();
		std::string receivedToken = "";
		std::string errorMessage;
		bool continueProcess = true;
		bool firstMessage = true;
		do
		{
			std::string token = client.generateToken(receivedToken);

			ot::JsonDocument tokenMessage;
			tokenMessage.AddMember(OT_ACTION_MEMBER, ot::JsonString(_authorisationServiceEndpoint, tokenMessage.GetAllocator()), tokenMessage.GetAllocator());
			tokenMessage.AddMember(OT_PARAM_AUTH_Token, ot::JsonString(token, tokenMessage.GetAllocator()), tokenMessage.GetAllocator());
			tokenMessage.AddMember(OT_PARAM_AUTH_USERNAME, ot::JsonString(_loginData.getUserName(), tokenMessage.GetAllocator()), tokenMessage.GetAllocator());
			tokenMessage.AddMember(OT_PARAM_AUTH_SSO_Initial, firstMessage, tokenMessage.GetAllocator());
			firstMessage = false;
			std::string response;
			if (ot::msg::send("", authorisationURL, ot::EXECUTE_ONE_WAY_TLS, tokenMessage.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit))
			{
				ot::JsonDocument responseDoc;
				responseDoc.fromJson(response);
				if (ot::json::getBool(responseDoc, OT_ACTION_AUTH_SUCCESS))
				{
					continueProcess = false;
					_processSuccessfulSSO(_loginData, responseDoc);
					return std::nullopt;
				}
				else
				{
					bool continueSequence = ot::json::getBool(responseDoc, OT_PARAM_AUTH_SSO_Token_Continue);

					if (continueSequence)
					{
						receivedToken = ot::json::getString(responseDoc, OT_PARAM_AUTH_Token);
						continueProcess = !receivedToken.empty();
						if (receivedToken.empty())
						{
							errorMessage = "Received empty authentication token.";
						}
					}
					else
					{
						errorMessage = ot::json::getString(responseDoc, OT_ACTION_PARAM_LOG);
						continueProcess = false;	
					}
				}
			}
			else
			{
				errorMessage = "Failed to connect to authorisation service";
				continueProcess = false;
			}
		} while (continueProcess);
		OT_LOG_E(errorMessage);
		return errorMessage;
	}
	catch (std::exception& _e)
	{
		OT_LOG_E(_e.what());
		return _e.what();
	}
}

std::optional<std::string> ot::Authentication::refreshToken(LoginData& _loginData)
{
	auto resetToken = [](LoginData& _loginData, ot::JsonDocument& _resultDoc)
		{
			std::string sessionToken = ot::json::getString(_resultDoc, OT_PARAM_AUTH_Token);
			_loginData.setSSOSessionToken(sessionToken);
		};
	std::optional<std::string> errorMessage = ssoSequence(_loginData, resetToken, OT_ACTION_SSO_Token_Refresh);
	return errorMessage;
}

std::optional<std::string> ot::Authentication::loginSSO(LoginData& _loginData)
{

	auto setLoginData = [](LoginData& _loginData, ot::JsonDocument& _resultDoc)
		{
			std::string sessionUser = ot::json::getString(_resultDoc, OT_PARAM_DB_USERNAME);
			std::string sessionPassword = ot::json::getString(_resultDoc, OT_PARAM_DB_PASSWORD);
			std::string sessionToken = ot::json::getString(_resultDoc, OT_PARAM_AUTH_Token);
			_loginData.setSSOSessionToken(sessionToken);
			_loginData.setSessionUser(sessionUser);
			_loginData.setSessionPassword(sessionPassword);
		};

	std::optional<std::string> errorMessage = ssoSequence(_loginData, setLoginData, OT_ACTION_LOGIN);
	return errorMessage;
}

bool ot::Authentication::validateAndRefreshToken(LoginData& _loginData)
{
	if (!_loginData.loggedInViaSSO())
	{
		return true; // Token validation only necessary if the login happened through sso
	}
	ot::JsonDocument validateMessage;
	validateMessage.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_SSO_Token_Validate, validateMessage.GetAllocator()), validateMessage.GetAllocator());
	validateMessage.AddMember(OT_PARAM_AUTH_Token, ot::JsonString(_loginData.getSSOSessionToken(), validateMessage.GetAllocator()), validateMessage.GetAllocator());
	validateMessage.AddMember(OT_PARAM_AUTH_USERNAME, ot::JsonString(_loginData.getUserName(), validateMessage.GetAllocator()), validateMessage.GetAllocator());
	std::string response;
	bool success = false;
	if (ot::msg::send("", _loginData.getAuthorizationUrl(), ot::EXECUTE_ONE_WAY_TLS, validateMessage.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit))
	{
		ot::JsonDocument responseDoc;
		responseDoc.fromJson(response);
		bool tokenIsValid = ot::json::getBool(responseDoc, OT_ACTION_AUTH_SUCCESS);
		if (!tokenIsValid)
		{

			std::optional<std::string> errorMessage = refreshToken(_loginData);
			// Error message is already logged in authenticateSSO
			success = ! errorMessage.has_value();
		}
		else
		{
			// Here the token is still valid
			success = true;
		}
	}
	return success;
}

void ot::Authentication::addAuthenticationData(const LoginData& _loginData, ot::JsonDocument& _doc)
{
	_doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(_loginData.getUserName(), _doc.GetAllocator()), _doc.GetAllocator());
	if (_loginData.loggedInViaSSO())
	{
		_doc.AddMember(OT_PARAM_AUTH_Token, ot::JsonString(_loginData.getSSOSessionToken(), _doc.GetAllocator()), _doc.GetAllocator());
	}
	else
	{
		_doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(_loginData.getUserPassword(), _doc.GetAllocator()), _doc.GetAllocator());
	}
}

