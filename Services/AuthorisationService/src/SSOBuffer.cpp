#include "SSOBuffer.h"
#include "OTCore/JSON/JSON.h"
#include "OTCore/ReturnMessage.h"
std::string SSOBuffer::handleRequest(const std::string& _username, const std::string& _receivedToken)
{
	ot::ReturnMessage returnMessage;
	try
	{
		auto serverSignOnByUsername = m_serverSignOnByUsername.find(_username);
		bool firstCall = false; // Could be removed if the state is carried in the SSO_Server class. This would make Kerberos possible as well
		if (serverSignOnByUsername == m_serverSignOnByUsername.end())
		{
			firstCall = true;
			m_serverSignOnByUsername.emplace(_username, std::make_unique<ot::SingleSignOn_Server>());
			serverSignOnByUsername = m_serverSignOnByUsername.find(_username);
		}

		std::unique_ptr<ot::SingleSignOn_Server >& serverSSO = serverSignOnByUsername->second;
		
		if (firstCall)
		{
			const std::string secondToken = serverSSO->processToken(_receivedToken);
			if(secondToken.empty())
			{ 
				const std::string stateString =	serverSSO->getStateString();
				returnMessage.setStatus(ot::ReturnMessage::Failed);
				returnMessage.setWhat("Server failed to process token. State: " + stateString);
			}
			else
			{
				returnMessage.setStatus(ot::ReturnMessage::Ok);
				returnMessage.setWhat(secondToken);
			}
		}
		else
		{
			
			serverSSO->processToken(_receivedToken);
			if (serverSSO->stateOK())
			{
				ot::LogInInfos infos = serverSSO->processLoggedInUserInfo();
				// Here we can load the MongoDB user now.

				if (infos.m_userName == _username)
				{
					returnMessage.setStatus(ot::ReturnMessage::Ok);
				}
				else
				{
					returnMessage.setStatus(ot::ReturnMessage::Failed);
					returnMessage.setWhat("User information does not match.");
				}
			}
			else if (! serverSSO->stateContinueNeeded())
			{
				returnMessage.setStatus(ot::ReturnMessage::Failed);
				returnMessage.setWhat("Server failed to process token. State: " + serverSSO->getStateString());
			}
		}

		return returnMessage.toJson();
	}
	catch (const std::exception& _e)
	{
		returnMessage.setStatus(ot::ReturnMessage::Failed);	
		const std::string  message = "Internal error: " + std::string(_e.what());
		returnMessage.setWhat(message);
		return returnMessage.toJson();
	}
}
