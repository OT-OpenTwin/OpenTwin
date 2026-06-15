#include "SSOBuffer.h"
#include "OTCore/JSON/JSON.h"
#include "OTCore/ReturnMessage.h"
#include "MongoUserFunctions.h"
#include "OTSystem/SingleSignOn/SingleSignOn_ReturnMessages.h"
#include "ServiceBase.h"

bool SSOBuffer::handleRequest(const std::string& _username, const std::string& _receivedToken, bool _initialToken, std::string& _returnMessage)
{
	ot::ReturnMessage returnMessage;
	bool authenticationSucceeded = false;
	try
	{
		auto serverSignOnByUsername = m_serverSignOnByUsername.find(_username);
		if (_initialToken)
		{
			m_serverSignOnByUsername.erase(_username);
			m_serverSignOnByUsername.emplace(_username, std::make_unique<ot::SingleSignOn_Server>());
			serverSignOnByUsername = m_serverSignOnByUsername.find(_username);
		}

		std::unique_ptr<ot::SingleSignOn_Server >& serverSSO = serverSignOnByUsername->second;
		
		const std::string nextToken = serverSSO->processToken(_receivedToken);
		if (serverSSO->stateContinueNeeded())
		{
			// Sequence needs to be continued return next token to client
			returnMessage.setWhat(nextToken);
			returnMessage.setStatus(ot::ReturnMessage::Ok);
		}
		else if(serverSSO->stateOK())
		{
			// Sequence concluded 
			ot::LogInInfos infos = serverSSO->processLoggedInUserInfo();
			// Here we can load the MongoDB user now.

			if (infos.m_userName == _username)
			{
				returnMessage.setStatus(ot::ReturnMessage::Ok);
				SSOUser user(infos);
				m_loggedInSSOUsersByUsername.insert(std::make_pair<>(_username, std::move(user)));
				authenticationSucceeded = true;
			}
			else
			{
				returnMessage.setStatus(ot::ReturnMessage::Failed);
				returnMessage.setWhat("Given username does not match identified name.");
			}
		}
		else
		{
			const std::string stateString =	serverSSO->getStateString();
			returnMessage.setStatus(ot::ReturnMessage::Failed);
			returnMessage.setWhat("Server failed to process token. State: " + stateString);
		}

		_returnMessage = returnMessage.toJson();
		return authenticationSucceeded;
	}
	catch (const std::exception& _e)
	{
		returnMessage.setStatus(ot::ReturnMessage::Failed);	
		const std::string  message = "Internal error: " + std::string(_e.what());
		returnMessage.setWhat(message);
		_returnMessage = returnMessage.toJson();
		return false;
	}
}

std::optional<std::string> SSOBuffer::getToken(const std::string& _userName)
{
	auto ssoUserByUserName = m_loggedInSSOUsersByUsername.find(_userName);
	return ssoUserByUserName->second.getSessionToken().getToken();
}
