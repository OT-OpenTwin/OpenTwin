#include "SSOBuffer.h"
#include "OTCore/JSON/JSON.h"
#include "MongoUserFunctions.h"
#include "ServiceBase.h"
#include "OTCommunication/ActionTypes.h"

bool SSOBuffer::handleRequest(const std::string& _username, const std::string& _receivedToken, bool _initialToken, ot::JsonDocument& _returnMessage)
{
	bool authenticationSucceeded = false;
	try
	{
		auto serverSignOnByUsername = m_serverSignOnByUsername.find(_username);
		if (_initialToken)
		{
			clearUser(_username);
			m_serverSignOnByUsername.emplace(_username, std::make_unique<ot::SingleSignOn_Server>());
			serverSignOnByUsername = m_serverSignOnByUsername.find(_username);
		}

		std::unique_ptr<ot::SingleSignOn_Server >& serverSSO = serverSignOnByUsername->second;
		
		const std::string nextToken = serverSSO->processToken(_receivedToken);
		if (serverSSO->stateContinueNeeded())
		{
			// Sequence needs to be continued return next token to client
			_returnMessage.AddMember(OT_ACTION_AUTH_SUCCESS, false, _returnMessage.GetAllocator());
			_returnMessage.AddMember(OT_PARAM_AUTH_SSO_Token_Continue, true, _returnMessage.GetAllocator());
			_returnMessage.AddMember(OT_PARAM_AUTH_Token, ot::JsonString(nextToken, _returnMessage.GetAllocator()), _returnMessage.GetAllocator());
		}
		else if(serverSSO->stateOK())
		{
			// Sequence concluded 
			ot::LogInInfos infos = serverSSO->processLoggedInUserInfo();
			// Here we can load the MongoDB user now.

			if (infos.m_userName == _username)
			{
				
				SSOUser user(infos);
				auto sessionToken = user.getSessionToken().getToken();
				assert(sessionToken.has_value()); // Freshly generated
				_returnMessage.AddMember(OT_ACTION_AUTH_SUCCESS, true, _returnMessage.GetAllocator());
				_returnMessage.AddMember(OT_PARAM_AUTH_SSO_Token_Continue, false, _returnMessage.GetAllocator());
				_returnMessage.AddMember(OT_PARAM_AUTH_Token, ot::JsonString(sessionToken.value(), _returnMessage.GetAllocator()), _returnMessage.GetAllocator());

				m_loggedInSSOUsersByUsername.insert(std::make_pair<>(_username, std::move(user)));
				authenticationSucceeded = true;
			}
			else
			{
				_returnMessage.AddMember(OT_ACTION_AUTH_SUCCESS, false, _returnMessage.GetAllocator());
				_returnMessage.AddMember(OT_ACTION_PARAM_LOG, ot::JsonString("Given username does not match identified name.", _returnMessage.GetAllocator()), _returnMessage.GetAllocator());			
			}
		}
		else
		{
			const std::string stateString =	serverSSO->getStateString();
			_returnMessage.AddMember(OT_ACTION_AUTH_SUCCESS, false, _returnMessage.GetAllocator());
			_returnMessage.AddMember(OT_ACTION_PARAM_LOG, ot::JsonString("Server failed to process token. State: " + stateString, _returnMessage.GetAllocator()), _returnMessage.GetAllocator());
		}

		return authenticationSucceeded;
	}
	catch (const std::exception& _e)
	{
		_returnMessage.AddMember(OT_ACTION_AUTH_SUCCESS, false, _returnMessage.GetAllocator());
		const std::string  message = "Internal error: " + std::string(_e.what());
		_returnMessage.AddMember(OT_ACTION_PARAM_LOG, ot::JsonString(message, _returnMessage.GetAllocator()), _returnMessage.GetAllocator());

		return false;
	}
}

std::optional<std::string> SSOBuffer::getToken(const std::string& _userName)
{
	auto ssoUserByUserName = m_loggedInSSOUsersByUsername.find(_userName);
	return ssoUserByUserName->second.getSessionToken().getToken();
}

void SSOBuffer::clearUser(const std::string& _username)
{
	m_serverSignOnByUsername.erase(_username);
	m_loggedInSSOUsersByUsername.erase(_username);
}
