#pragma once
#include <iostream>
#include <string>
#include <bsoncxx/types/value.hpp>

using value = bsoncxx::types::value;

namespace DataStorageAPI
{
	class __declspec(dllexport) BsonValuesHelper
	{
	public:
		static value getInt32BsonValue(int32_t value);
		static int32_t getInt32FromBsonValue(value value);

		static value getInt64BsonValue(int64_t value);
		static int64_t getInt64FromBsonValue(value value);

		static std::string getStringFromBsonValue(value value);

		static value getBoolValue(bool value);
		static bool getBoolFromBsonValue(value value);

		static value getDoubleValue(double value);
		static double getDoubleFromBsonValue(value value);

		static value getOIdValue(std::string value);
		static std::string getOIdFromBsonValue(value value);

	private:
		BsonValuesHelper() = default;
	};
}

