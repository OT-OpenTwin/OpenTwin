
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

		mongocxx::database secondaryDb = adminClient.database(MongoConstants::USER_DB);
		mongocxx::collection sessionsCollection = secondaryDb.collection(MongoConstants::SESSIONS_COLLECTION);

		value session_value = document{}
			<< "session_name" << sessionName
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

}
