// @otlicense

#include "stdafx.h"
#include "..\include\Helper\BsonValuesHelper.h"
#include "bsoncxx/types/bson_value/make_value.hpp"
#include "bsoncxx/string/view_or_value.hpp"

namespace DataStorageAPI
{
	value BsonValuesHelper::getInt32BsonValue(int32_t value)
	{
		bsoncxx::types::b_int32 bInt{ value };
		return bsoncxx::types::value{ bInt };
	}

	int32_t BsonValuesHelper::getInt32FromBsonValue(value value)
	{
		return value.get_int32().value;
	}

	value BsonValuesHelper::getInt64BsonValue(int64_t value)
	{
		bsoncxx::types::b_int64 bInt{ value };
		return bsoncxx::types::value{ bInt };
	}

	int64_t BsonValuesHelper::getInt64FromBsonValue(value value)
	{
		return value.get_int64().value;
	}

	std::string BsonValuesHelper::getStringFromBsonValue(value value)
	{
		return std::string(value.get_utf8().value.data());
	}

	value BsonValuesHelper::getBoolValue(bool value)
	{
		bsoncxx::types::b_bool bBool{ value };
		return bsoncxx::types::value{ bBool };
	}
	bool BsonValuesHelper::getBoolFromBsonValue(value value)
	{
		return value.get_bool().value;
	}
	value BsonValuesHelper::getDoubleValue(double value)
	{
		bsoncxx::types::b_double bDouble{ value };
		return bsoncxx::types::value{ bDouble };
	}
	double BsonValuesHelper::getDoubleFromBsonValue(value value)
	{
		return value.get_double().value;
	}

	value BsonValuesHelper::getOIdValue(std::string value)
	{
		bsoncxx::types::b_oid id{ bsoncxx::oid { value } };
		return bsoncxx::types::value{ id };
	}

	std::string BsonValuesHelper::getOIdFromBsonValue(value value)
	{
		return value.get_oid().value.to_string();
	}
}
