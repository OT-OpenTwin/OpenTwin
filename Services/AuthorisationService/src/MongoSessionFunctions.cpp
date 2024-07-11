
#include "MongoSessionFunctions.h"
#include "MongoRoleFunctions.h"
#include "MongoConstants.h"
#include "MongoUserFunctions.h"
#include "OTCore/JSON.h"

#include <sstream>


namespace MongoSessionFunctions
{
	std::string createSession(std::string username, mongocxx::client& adminClient)
	{
		std::string sessionName = generateUniqueSessionName(username, adminClient);
		std::time_t currentTime = std::time(nullptr);

		mongocxx::database secondaryDb = adminClient.database(MongoConstants::USER_DB);
		mongocxx::collection sessionsCollection = secondaryDb.collection(MongoConstants::SESSIONS_COLLECTION);

		value session_value = document{}
			<< "session_name" << sessionName
			<< "time_stamp" << currentTime
			<< finalize;

		sessionsCollection.insert_one(session_value.view());

		return sessionName;
	}

	std::string generateUniqueSessionName(std::string username, mongocxx::client& adminClient)
	{
		std::ostringstream sessionName;

		bool done = false;

		while (!done)
		{
			std::time_t currentTime = std::time(nullptr);

			struct tm currentDate;
			localtime_s(&currentDate, &currentTime);
			int random = rand() % 100;

			sessionName.clear();
			sessionName << username << "-" << 1900 + currentDate.tm_year << currentDate.tm_mon + 1 << currentDate.tm_mday << "-"
				<< currentDate.tm_hour << currentDate.tm_min << currentDate.tm_sec << "-" << random;

			done = true;

			mongocxx::database secondaryDb = adminClient.database(MongoConstants::USER_DB);
			mongocxx::collection sessionsCollection = secondaryDb.collection(MongoConstants::SESSIONS_COLLECTION);

			value find_session_query = document{}
				<< "session_name" << sessionName.str()
				<< finalize;

			auto sessionOptional = sessionsCollection.find_one(find_session_query.view());

			if (sessionOptional)
			{
				done = false;
			}
		}

		return sessionName.str();
	}

	std::string refreshSession(std::string sessionName, mongocxx::client& adminClient)
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

	void removeOldSessions(mongocxx::client& adminClient)
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
}


