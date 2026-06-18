#include "SSOBuffer.h"
#include "OTCore/JSON/JSON.h"
#include "MongoUserFunctions.h"
#include "ServiceBase.h"
#include "OTCommunication/ActionTypes.h"

bool SSOBuffer::handleRequest(std::string& _determinedUsername, const std::string& _authenticationProcessID,  const std::string& _receivedToken, bool _initialToken, ot::JsonDocument& _returnMessage)
{
	clearExpiredProcessIDs();
	bool authenticationSucceeded = false;

	try
	{
		ot::AuthenticationProcessID* processID = nullptr;
		std::unordered_map<ot::AuthenticationProcessID, std::unique_ptr<ot::SingleSignOn_Server>>::iterator serverSignOnByProcessID;
		if (_initialToken)
		{
			auto entry = m_serverSignOnByProcessID.emplace(ot::AuthenticationProcessID(), std::make_unique<ot::SingleSignOn_Server>());
			assert(entry.second);
			serverSignOnByProcessID = entry.first;
		}
		else
		{
			serverSignOnByProcessID = m_serverSignOnByProcessID.find(_authenticationProcessID);
		}

		if (serverSignOnByProcessID == m_serverSignOnByProcessID.end())
		{
			_returnMessage.AddMember(OT_ACTION_AUTH_SUCCESS, false, _returnMessage.GetAllocator());
			_returnMessage.AddMember(OT_ACTION_PARAM_LOG, ot::JsonString("Failed to assigning authentication process.", _returnMessage.GetAllocator()), _returnMessage.GetAllocator());
			return false;
		}
		
		std::unique_ptr<ot::SingleSignOn_Server >& serverSSO = serverSignOnByProcessID->second;
		const std::string nextToken = serverSSO->processToken(_receivedToken);
		if (serverSSO->stateContinueNeeded())
		{
			// Sequence needs to be continued return next token to client
			_returnMessage.AddMember(OT_ACTION_AUTH_SUCCESS, false, _returnMessage.GetAllocator());
			_returnMessage.AddMember(OT_PARAM_AUTH_SSO_Token_Continue, true, _returnMessage.GetAllocator());
			_returnMessage.AddMember(OT_PARAM_AUTH_Token, ot::JsonString(nextToken, _returnMessage.GetAllocator()), _returnMessage.GetAllocator());
			_returnMessage.AddMember(OT_ACTION_PARAM_PROCESS_ID, ot::JsonString(serverSignOnByProcessID->first.getID(), _returnMessage.GetAllocator()), _returnMessage.GetAllocator());
		}
		else if(serverSSO->stateOK())
		{
			// Sequence concluded 
			ot::LogInInfos infos = serverSSO->processLoggedInUserInfo();
			// Here we can load the MongoDB user now.
			_determinedUsername = infos.m_userName;
				
			SSOUser user(infos);
			const std::string& sessionToken = user.getSessionToken().getToken();
			
			_returnMessage.AddMember(OT_ACTION_AUTH_SUCCESS, true, _returnMessage.GetAllocator());
			_returnMessage.AddMember(OT_PARAM_AUTH_SSO_Token_Continue, false, _returnMessage.GetAllocator());
			_returnMessage.AddMember(OT_PARAM_AUTH_Token, ot::JsonString(sessionToken, _returnMessage.GetAllocator()), _returnMessage.GetAllocator());
			_returnMessage.AddMember(OT_PARAM_AUTH_USERNAME, ot::JsonString(_determinedUsername, _returnMessage.GetAllocator()), _returnMessage.GetAllocator());

			m_loggedInSSOUsersByToken.insert(std::make_pair<>(sessionToken, std::move(user)));
			m_serverSignOnByProcessID.erase(serverSignOnByProcessID);
			authenticationSucceeded = true;

		}
		else
		{
			const std::string stateString =	serverSSO->getStateString();
			_returnMessage.AddMember(OT_ACTION_AUTH_SUCCESS, false, _returnMessage.GetAllocator());
			_returnMessage.AddMember(OT_ACTION_PARAM_LOG, ot::JsonString("Server failed to process token. State: " + stateString, _returnMessage.GetAllocator()), _returnMessage.GetAllocator());
			m_serverSignOnByProcessID.erase(serverSignOnByProcessID);
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

SSOUser* SSOBuffer::getUser(const std::string& _token)
{
	auto loggedInSSOUsersByToken =	m_loggedInSSOUsersByToken.find(_token);
	if (loggedInSSOUsersByToken == m_loggedInSSOUsersByToken.end())
	{
		return nullptr;
	}
	else
	{
		return &loggedInSSOUsersByToken->second;
	}
}

void SSOBuffer::removeUser(const std::string& _token)
{
	m_loggedInSSOUsersByToken.erase(_token);
}


void SSOBuffer::clearExpiredProcessIDs()
{
	
	for (auto entry = m_serverSignOnByProcessID.begin(); entry != m_serverSignOnByProcessID.end(); )
	{
		if (!entry->first.isIsExpired())
		{
			entry = m_serverSignOnByProcessID.erase(entry);
		}
		else
		{
			entry++;
		}
	}
}

bool SSOBuffer::validate(const std::string& _token) 
{
	auto loggedInSSOUserByToken= m_loggedInSSOUsersByToken.find(_token);
	bool isValid = false;
	if (loggedInSSOUserByToken == m_loggedInSSOUsersByToken.end())
	{
		assert(false); // This spot should only be accessed by the UI after the login
	}
	else
	{
		SSOUser& user = loggedInSSOUserByToken->second;
		isValid = user.getSessionToken().tokenIsValid();
	}
	return isValid;
}
