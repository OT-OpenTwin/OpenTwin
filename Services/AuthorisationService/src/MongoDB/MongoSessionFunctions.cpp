// @otlicense
// File: MongoSessionFunctions.cpp
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

// Auth header
#include "MongoDB/MongoConstants.h"
#include "MongoDB/MongoRoleFunctions.h"
#include "MongoDB/MongoUserFunctions.h"
#include "MongoDB/MongoSessionFunctions.h"

// OpenTwin header
#include "OTCore/JSON/JSON.h"
#include "OTServiceFoundation/Encryption.h"

// std header
#include <sstream>

std::string MongoSessionFunctions::createSession(const std::string& _username, const std::string& _sessionToken, mongocxx::client& _adminClient)
{
	std::string sessionName = generateUniqueSessionName(_username, _adminClient);
	std::time_t currentTime = std::time(nullptr);

	mongocxx::database secondaryDb = _adminClient.database(MongoConstants::USER_DB);
	mongocxx::collection sessionsCollection = secondaryDb.collection(MongoConstants::SESSIONS_COLLECTION);

	std::string encryptedSessionToken = ot::Encryption::encryptString(_sessionToken);

	value session_value = document{}
		<< "session_name" << sessionName
		<< "session_token" << encryptedSessionToken
		<< "time_stamp" << currentTime
		<< finalize;

	sessionsCollection.insert_one(session_value.view());

	return sessionName;
}

std::string MongoSessionFunctions::generateUniqueSessionName(std::string username, mongocxx::client& adminClient)
{
	std::string newSessionName;

	while (newSessionName.empty())
	{
		std::time_t currentTime = std::time(nullptr);

		struct tm currentDate;
		localtime_s(&currentDate, &currentTime);
		int random = rand() % 100;

		std::ostringstream sessionNameStream;

		sessionNameStream << username << "-" << 1900 + currentDate.tm_year << currentDate.tm_mon + 1 << currentDate.tm_mday << "-"
			<< currentDate.tm_hour << currentDate.tm_min << currentDate.tm_sec << "-" << random;

		newSessionName = sessionNameStream.str();

		mongocxx::database secondaryDb = adminClient.database(MongoConstants::USER_DB);
		mongocxx::collection sessionsCollection = secondaryDb.collection(MongoConstants::SESSIONS_COLLECTION);

		value find_session_query = document{}
			<< "session_name" << newSessionName
			<< finalize;

		auto sessionOptional = sessionsCollection.find_one(find_session_query.view());

		if (sessionOptional)
		{
			newSessionName.clear();
		}
	}

	return newSessionName;
}

std::string MongoSessionFunctions::refreshSession(std::string sessionName, mongocxx::client& adminClient)
{
	std::time_t currentTime = std::time(nullptr);

	value sessionFilter = document{}
		<< "session_name" << sessionName
		<< finalize;

	value sessionUpdate = document{}
		<< "$set"
		<< open_document
		<< "time_stamp" << currentTime
		<< close_document
		<< finalize;

	auto sessionsCollection = adminClient.database(MongoConstants::USER_DB).collection(MongoConstants::SESSIONS_COLLECTION);
	auto result = sessionsCollection.update_one(sessionFilter.view(), sessionUpdate.view());

	return "";
}

void MongoSessionFunctions::removeOldSessions(mongocxx::client& adminClient)
{
	// Remove the session if it has not been touched (refreshed) for 24 hours.
	auto gracePeriod = std::chrono::hours(24);
	auto currentTime = std::chrono::system_clock::now();
	time_t validSessionTime = std::chrono::system_clock::to_time_t(currentTime - gracePeriod);

	auto queryDoc = bsoncxx::builder::stream::document{}
		<< "time_stamp" << bsoncxx::builder::stream::open_document << "$lt" << validSessionTime
		<< bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize;

	auto sessionsCollection = adminClient.database(MongoConstants::USER_DB).collection(MongoConstants::SESSIONS_COLLECTION);
	auto result = sessionsCollection.find(queryDoc.view());

	size_t sessionCount = 0;

	auto array_builder = bsoncxx::builder::basic::array{};

	try
	{
		for (auto item : result)
		{
			std::string sessionName = item["session_name"].get_utf8().value.data();

			MongoUserFunctions::removeTmpUser(sessionName, adminClient);
			array_builder.append(sessionName);

			sessionCount++;
		}
	}
	catch (std::exception)
	{
	}

	if (sessionCount > 0)
	{
		auto deleteDoc = bsoncxx::builder::stream::document{}
			<< "session_name" << bsoncxx::builder::stream::open_document
			<< "$in" << array_builder
			<< bsoncxx::builder::stream::close_document
			<< bsoncxx::builder::stream::finalize;

		sessionsCollection.delete_many(deleteDoc.view());
	}
}


