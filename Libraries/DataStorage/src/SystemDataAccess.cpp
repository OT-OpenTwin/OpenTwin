// @otlicense

#include "stdafx.h"

#include <map>
#include <vector>
#include "bsoncxx/types/value.hpp"
#include "bsoncxx/json.hpp"
#include "../include/System/SystemDataAccess.h"
#include "../include/Helper/QueryBuilder.h"
#include "../include/Document/DocumentAccessBase.h"

using value = bsoncxx::types::value;

namespace DataStorageAPI
{
	SystemDataAccess::SystemDataAccess(string databaseName, string collectionName)
	{
		_mongoDbName = databaseName;
		_mongoCollectionName = collectionName;
	}

	DataStorageResponse SystemDataAccess::GetSystemDetails(int siteId)
	{
		DataStorageResponse response;
		try
		{
			std::map<std::string, value> filterKvps;
			bsoncxx::types::value siteIdVal(bsoncxx::types::b_int32{ siteId });

			filterKvps.insert(std::pair<std::string, value>("SiteId", siteIdVal));

			QueryBuilder queryBuilder;
			auto filterQuery = queryBuilder.GenerateFilterQuery(filterKvps);
			auto projectileQuery = queryBuilder.GenerateSelectQuery(std::vector<std::string> {}, true);

			DocumentAccessBase systemAccess(_mongoDbName, _mongoCollectionName);
			auto result = systemAccess.GetDocument(std::move(filterQuery), std::move(projectileQuery));
			if (result)
			{
				response.setBsonResult(result);
				response.updateDataStorageResponse(bsoncxx::to_json(*result), true, "");
			}
			else
			{
				response.updateDataStorageResponse("", false, "Data not found!");
			}
			return response;
		}
		catch (std::exception& e)
		{
			std::cout << e.what();
			response.updateDataStorageResponse("", false, e.what());
			return response;
		}
	}
}
