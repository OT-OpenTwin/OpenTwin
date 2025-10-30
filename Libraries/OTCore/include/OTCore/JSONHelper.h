// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/JSONTypes.h"
#include "OTCore/JSONDocument.h"
#include "OTCore/CoreTypes.h"

// std header
#include <string>
#include <vector>

namespace ot {

	//! @brief Provides helper functions to acces content of json objects and arrays
	namespace json {

		//! @brief Returns true if the specified object member exists
		OT_CORE_API_EXPORT bool exists(const JsonValue& _value, const char* _member);
		OT_CORE_API_EXPORT bool exists(const JsonValue& _value, const std::string& _member);

		//! @brief Returns true if the specified object member exists
		OT_CORE_API_EXPORT bool exists(const ConstJsonObject& _value, const char* _member);
		OT_CORE_API_EXPORT bool exists(const ConstJsonObject& _value, const std::string& _member);

		//! @brief Returns true if the specified object member is a null
		OT_CORE_API_EXPORT bool isNull(const JsonValue& _value, const char* _member);
		OT_CORE_API_EXPORT bool isNull(const JsonValue& _value, const std::string& _member);
		//! @brief Returns true if the specified object member is a null
		OT_CORE_API_EXPORT bool isNull(const ConstJsonObject& _value, const char* _member);
		OT_CORE_API_EXPORT bool isNull(const ConstJsonObject& _value, const std::string& _member);

		//! @brief Returns true if the specified object member is a bool
		OT_CORE_API_EXPORT bool isBool(const JsonValue& _value, const char* _member);
		OT_CORE_API_EXPORT bool isBool(const JsonValue& _value, const std::string& _member);
		//! @brief Returns true if the specified object member is a bool
		OT_CORE_API_EXPORT bool isBool(const ConstJsonObject& _value, const char* _member);
		OT_CORE_API_EXPORT bool isBool(const ConstJsonObject& _value, const std::string& _member);

		//! @brief Returns true if the specified object member is a int
		OT_CORE_API_EXPORT bool isInt(const JsonValue& _value, const char* _member);
		OT_CORE_API_EXPORT bool isInt(const JsonValue& _value, const std::string& _member);
		//! @brief Returns true if the specified object member is a int
		OT_CORE_API_EXPORT bool isInt(const ConstJsonObject& _value, const char* _member);
		OT_CORE_API_EXPORT bool isInt(const ConstJsonObject& _value, const std::string& _member);

		//! @brief Returns true if the specified object member is a uInt
		OT_CORE_API_EXPORT bool isUInt(const JsonValue& _value, const char* _member);
		OT_CORE_API_EXPORT bool isUInt(const JsonValue& _value, const std::string& _member);
		//! @brief Returns true if the specified object member is a uInt
		OT_CORE_API_EXPORT bool isUInt(const ConstJsonObject& _value, const char* _member);
		OT_CORE_API_EXPORT bool isUInt(const ConstJsonObject& _value, const std::string& _member);

		//! @brief Returns true if the specified object member is a int64
		OT_CORE_API_EXPORT bool isInt64(const JsonValue& _value, const char* _member);
		OT_CORE_API_EXPORT bool isInt64(const JsonValue& _value, const std::string& _member);
		//! @brief Returns true if the specified object member is a int64
		OT_CORE_API_EXPORT bool isInt64(const ConstJsonObject& _value, const char* _member);
		OT_CORE_API_EXPORT bool isInt64(const ConstJsonObject& _value, const std::string& _member);

		//! @brief Returns true if the specified object member is a uInt64
		OT_CORE_API_EXPORT bool isUInt64(const JsonValue& _value, const char* _member);
		OT_CORE_API_EXPORT bool isUInt64(const JsonValue& _value, const std::string& _member);
		//! @brief Returns true if the specified object member is a uInt64
		OT_CORE_API_EXPORT bool isUInt64(const ConstJsonObject& _value, const char* _member);
		OT_CORE_API_EXPORT bool isUInt64(const ConstJsonObject& _value, const std::string& _member);

		//! @brief Returns true if the specified object member is a float
		OT_CORE_API_EXPORT bool isFloat(const JsonValue& _value, const char* _member);
		OT_CORE_API_EXPORT bool isFloat(const JsonValue& _value, const std::string& _member);
		//! @brief Returns true if the specified object member is a float
		OT_CORE_API_EXPORT bool isFloat(const ConstJsonObject& _value, const char* _member);
		OT_CORE_API_EXPORT bool isFloat(const ConstJsonObject& _value, const std::string& _member);

		//! @brief Returns true if the specified object member is a double
		OT_CORE_API_EXPORT bool isDouble(const JsonValue& _value, const char* _member);
		OT_CORE_API_EXPORT bool isDouble(const JsonValue& _value, const std::string& _member);
		//! @brief Returns true if the specified object member is a double
		OT_CORE_API_EXPORT bool isDouble(const ConstJsonObject& _value, const char* _member);
		OT_CORE_API_EXPORT bool isDouble(const ConstJsonObject& _value, const std::string& _member);

		//! @brief Returns true if the specified object member is a string
		OT_CORE_API_EXPORT bool isString(const JsonValue& _value, const char* _member);
		OT_CORE_API_EXPORT bool isString(const JsonValue& _value, const std::string& _member);
		//! @brief Returns true if the specified object member is a string
		OT_CORE_API_EXPORT bool isString(const ConstJsonObject& _value, const char* _member);
		OT_CORE_API_EXPORT bool isString(const ConstJsonObject& _value, const std::string& _member);

		//! @brief Returns true if the specified object member is a object
		OT_CORE_API_EXPORT bool isObject(const JsonValue& _value, const char* _member);
		OT_CORE_API_EXPORT bool isObject(const JsonValue& _value, const std::string& _member);
		//! @brief Returns true if the specified object member is a object
		OT_CORE_API_EXPORT bool isObject(const ConstJsonObject& _value, const char* _member);
		OT_CORE_API_EXPORT bool isObject(const ConstJsonObject& _value, const std::string& _member);

		//! @brief Returns true if the specified object member is a array
		OT_CORE_API_EXPORT bool isArray(const JsonValue& _value, const char* _member);
		OT_CORE_API_EXPORT bool isArray(const JsonValue& _value, const std::string& _member);
		//! @brief Returns true if the specified object member is a array
		OT_CORE_API_EXPORT bool isArray(const ConstJsonObject& _value, const char* _member);
		OT_CORE_API_EXPORT bool isArray(const ConstJsonObject& _value, const std::string& _member);

		//! @brief Check and return the requested value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT bool getBool(const JsonValue& _value, const char* _member, bool _default = false);
		OT_CORE_API_EXPORT bool getBool(const JsonValue& _value, const std::string& _member, bool _default = false);
		OT_CORE_API_EXPORT bool getBool(const ConstJsonObject& _value, const char* _member, bool _default = false);
		OT_CORE_API_EXPORT bool getBool(const ConstJsonObject& _value, const std::string& _member, bool _default = false);

		//! @brief Check and return the requested value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT int32_t getInt(const JsonValue& _value, const char* _member, int32_t _default = 0);
		OT_CORE_API_EXPORT int32_t getInt(const JsonValue& _value, const std::string& _member, int32_t _default = 0);
		OT_CORE_API_EXPORT int32_t getInt(const ConstJsonObject& _value, const char* _member, int32_t _default = 0);
		OT_CORE_API_EXPORT int32_t getInt(const ConstJsonObject& _value, const std::string& _member, int32_t _default = 0);

		//! @brief Check and return the requested value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT uint32_t getUInt(const JsonValue& _value, const char* _member, uint32_t _default = 0);
		OT_CORE_API_EXPORT uint32_t getUInt(const JsonValue& _value, const std::string& _member, uint32_t _default = 0);
		OT_CORE_API_EXPORT uint32_t getUInt(const ConstJsonObject& _value, const char* _member, uint32_t _default = 0);
		OT_CORE_API_EXPORT uint32_t getUInt(const ConstJsonObject& _value, const std::string& _member, uint32_t _default = 0);

		//! @brief Check and return the requested value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT int64_t getInt64(const JsonValue& _value, const char* _member, int64_t _default = 0);
		OT_CORE_API_EXPORT int64_t getInt64(const JsonValue& _value, const std::string& _member, int64_t _default = 0);
		OT_CORE_API_EXPORT int64_t getInt64(const ConstJsonObject& _value, const char* _member, int64_t _default = 0);
		OT_CORE_API_EXPORT int64_t getInt64(const ConstJsonObject& _value, const std::string& _member, int64_t _default = 0);

		//! @brief Check and return the requested value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT uint64_t getUInt64(const JsonValue& _value, const char* _member, uint64_t _default = 0);
		OT_CORE_API_EXPORT uint64_t getUInt64(const JsonValue& _value, const std::string& _member, uint64_t _default = 0);
		OT_CORE_API_EXPORT uint64_t getUInt64(const ConstJsonObject& _value, const char* _member, uint64_t _default = 0);
		OT_CORE_API_EXPORT uint64_t getUInt64(const ConstJsonObject& _value, const std::string& _member, uint64_t _default = 0);

		//! @brief Check and return the requested value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT float getFloat(const JsonValue& _value, const char* _member, float _default = 0.f);
		OT_CORE_API_EXPORT float getFloat(const JsonValue& _value, const std::string& _member, float _default = 0.f);
		OT_CORE_API_EXPORT float getFloat(const ConstJsonObject& _value, const char* _member, float _default = 0.f);
		OT_CORE_API_EXPORT float getFloat(const ConstJsonObject& _value, const std::string& _member, float _default = 0.f);

		//! @brief Check and return the requested value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT double getDouble(const JsonValue& _value, const char* _member, double _default = 0.);
		OT_CORE_API_EXPORT double getDouble(const JsonValue& _value, const std::string& _member, double _default = 0.);
		OT_CORE_API_EXPORT double getDouble(const ConstJsonObject& _value, const char* _member, double _default = 0.);
		OT_CORE_API_EXPORT double getDouble(const ConstJsonObject& _value, const std::string& _member, double _default = 0.);

		//! @brief Check and return the requested value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT std::string getString(const JsonValue& _value, const char* _member, const std::string& _default = std::string());
		OT_CORE_API_EXPORT std::string getString(const JsonValue& _value, const std::string& _member, const std::string& _default = std::string());
		OT_CORE_API_EXPORT std::string getString(const ConstJsonObject& _value, const char* _member, const std::string& _default = std::string());
		OT_CORE_API_EXPORT std::string getString(const ConstJsonObject& _value, const std::string& _member, const std::string& _default = std::string());

		//! @brief Check and return the requested value
		//! Asserts, logs and throws when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT ConstJsonObject getObject(const JsonValue& _value, const char* _member);
		OT_CORE_API_EXPORT ConstJsonObject getObject(const JsonValue& _value, const std::string& _member);
		OT_CORE_API_EXPORT ConstJsonObject getObject(const ConstJsonObject& _value, const char* _member);
		OT_CORE_API_EXPORT ConstJsonObject getObject(const ConstJsonObject& _value, const std::string& _member);

		//! @brief Check and return the requested value
		//! Asserts, logs and throws when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT ConstJsonArray getArray(const JsonValue& _value, const char* _member);
		OT_CORE_API_EXPORT ConstJsonArray getArray(const JsonValue& _value, const std::string& _member);
		OT_CORE_API_EXPORT ConstJsonArray getArray(const ConstJsonObject& _value, const char* _member);
		OT_CORE_API_EXPORT ConstJsonArray getArray(const ConstJsonObject& _value, const std::string& _member);

		//! @brief Check and return the requested value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT std::list<bool> getBoolList(const JsonValue& _value, const char* _member, const std::list<bool>& _default = std::list<bool>());
		OT_CORE_API_EXPORT std::list<bool> getBoolList(const JsonValue& _value, const std::string& _member, const std::list<bool>& _default = std::list<bool>());
		OT_CORE_API_EXPORT std::list<bool> getBoolList(const ConstJsonObject& _value, const char* _member, const std::list<bool>& _default = std::list<bool>());
		OT_CORE_API_EXPORT std::list<bool> getBoolList(const ConstJsonObject& _value, const std::string& _member, const std::list<bool>& _default = std::list<bool>());

		//! @brief Check and return the requested value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT std::list<int32_t> getIntList(const JsonValue& _value, const char* _member, const std::list<int32_t>& _default = std::list<int32_t>());
		OT_CORE_API_EXPORT std::list<int32_t> getIntList(const JsonValue& _value, const std::string& _member, const std::list<int32_t>& _default = std::list<int32_t>());
		OT_CORE_API_EXPORT std::list<int32_t> getIntList(const ConstJsonObject& _value, const char* _member, const std::list<int32_t>& _default = std::list<int32_t>());
		OT_CORE_API_EXPORT std::list<int32_t> getIntList(const ConstJsonObject& _value, const std::string& _member, const std::list<int32_t>& _default = std::list<int32_t>());

		//! @brief Check and return the requested value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT std::list<uint32_t> getUIntList(const JsonValue& _value, const char* _member, const std::list<uint32_t>& _default = std::list<uint32_t>());
		OT_CORE_API_EXPORT std::list<uint32_t> getUIntList(const JsonValue& _value, const std::string& _member, const std::list<uint32_t>& _default = std::list<uint32_t>());
		OT_CORE_API_EXPORT std::list<uint32_t> getUIntList(const ConstJsonObject& _value, const char* _member, const std::list<uint32_t>& _default = std::list<uint32_t>());
		OT_CORE_API_EXPORT std::list<uint32_t> getUIntList(const ConstJsonObject& _value, const std::string& _member, const std::list<uint32_t>& _default = std::list<uint32_t>());

		//! @brief Check and return the requested value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT std::list<int64_t> getInt64List(const JsonValue& _value, const char* _member, const std::list<int64_t>& _default = std::list<int64_t>());
		OT_CORE_API_EXPORT std::list<int64_t> getInt64List(const JsonValue& _value, const std::string& _member, const std::list<int64_t>& _default = std::list<int64_t>());
		OT_CORE_API_EXPORT std::list<int64_t> getInt64List(const ConstJsonObject& _value, const char* _member, const std::list<int64_t>& _default = std::list<int64_t>());
		OT_CORE_API_EXPORT std::list<int64_t> getInt64List(const ConstJsonObject& _value, const std::string& _member, const std::list<int64_t>& _default = std::list<int64_t>());

		//! @brief Check and return the requested value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT std::list<uint64_t> getUInt64List(const JsonValue& _value, const char* _member, const std::list<uint64_t>& _default = std::list<uint64_t>());
		OT_CORE_API_EXPORT std::list<uint64_t> getUInt64List(const JsonValue& _value, const std::string& _member, const std::list<uint64_t>& _default = std::list<uint64_t>());
		OT_CORE_API_EXPORT std::list<uint64_t> getUInt64List(const ConstJsonObject& _value, const char* _member, const std::list<uint64_t>& _default = std::list<uint64_t>());
		OT_CORE_API_EXPORT std::list<uint64_t> getUInt64List(const ConstJsonObject& _value, const std::string& _member, const std::list<uint64_t>& _default = std::list<uint64_t>());

		//! @brief Check and return the requested value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT std::list<float> getFloatList(const JsonValue& _value, const char* _member, const std::list<float>& _default = std::list<float>());
		OT_CORE_API_EXPORT std::list<float> getFloatList(const JsonValue& _value, const std::string& _member, const std::list<float>& _default = std::list<float>());
		OT_CORE_API_EXPORT std::list<float> getFloatList(const ConstJsonObject& _value, const char* _member, const std::list<float>& _default = std::list<float>());
		OT_CORE_API_EXPORT std::list<float> getFloatList(const ConstJsonObject& _value, const std::string& _member, const std::list<float>& _default = std::list<float>());

		//! @brief Check and return the requested value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT std::list<double> getDoubleList(const JsonValue& _value, const char* _member, const std::list<double>& _default = std::list<double>());
		OT_CORE_API_EXPORT std::list<double> getDoubleList(const JsonValue& _value, const std::string& _member, const std::list<double>& _default = std::list<double>());
		OT_CORE_API_EXPORT std::list<double> getDoubleList(const ConstJsonObject& _value, const char* _member, const std::list<double>& _default = std::list<double>());
		OT_CORE_API_EXPORT std::list<double> getDoubleList(const ConstJsonObject& _value, const std::string& _member, const std::list<double>& _default = std::list<double>());

		//! @brief Check and return the requested value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT std::list<std::string> getStringList(const JsonValue& _value, const char* _member, const std::list<std::string>& _default = std::list<std::string>());
		OT_CORE_API_EXPORT std::list<std::string> getStringList(const JsonValue& _value, const std::string& _member, const std::list<std::string>& _default = std::list<std::string>());
		OT_CORE_API_EXPORT std::list<std::string> getStringList(const ConstJsonObject& _value, const char* _member, const std::list<std::string>& _default = std::list<std::string>());
		OT_CORE_API_EXPORT std::list<std::string> getStringList(const ConstJsonObject& _value, const std::string& _member, const std::list<std::string>& _default = std::list<std::string>());

		//! @brief Check and return the requested value
		//! Asserts, logs and throws when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT std::list<ConstJsonObject> getObjectList(const JsonValue& _value, const char* _member);
		OT_CORE_API_EXPORT std::list<ConstJsonObject> getObjectList(const JsonValue& _value, const std::string& _member);
		OT_CORE_API_EXPORT std::list<ConstJsonObject> getObjectList(const ConstJsonObject& _value, const char* _member);
		OT_CORE_API_EXPORT std::list<ConstJsonObject> getObjectList(const ConstJsonObject& _value, const std::string& _member);

		//! @brief Check and return the requested value
		//! Asserts, logs and throws when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT std::list<ConstJsonArray> getArrayList(const JsonValue& _value, const char* _member);
		OT_CORE_API_EXPORT std::list<ConstJsonArray> getArrayList(const JsonValue& _value, const std::string& _member);
		OT_CORE_API_EXPORT std::list<ConstJsonArray> getArrayList(const ConstJsonObject& _value, const char* _member);
		OT_CORE_API_EXPORT std::list<ConstJsonArray> getArrayList(const ConstJsonObject& _value, const std::string& _member);

		//! @brief Check and return the requested value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT std::vector<bool> getBoolVector(const JsonValue& _value, const char* _member, const std::vector<bool>& _default = std::vector<bool>());
		OT_CORE_API_EXPORT std::vector<bool> getBoolVector(const JsonValue& _value, const std::string& _member, const std::vector<bool>& _default = std::vector<bool>());
		OT_CORE_API_EXPORT std::vector<bool> getBoolVector(const ConstJsonObject& _value, const char* _member, const std::vector<bool>& _default = std::vector<bool>());
		OT_CORE_API_EXPORT std::vector<bool> getBoolVector(const ConstJsonObject& _value, const std::string& _member, const std::vector<bool>& _default = std::vector<bool>());

		//! @brief Check and return the requested value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT std::vector<int32_t> getIntVector(const JsonValue& _value, const char* _member, const std::vector<int32_t>& _default = std::vector<int32_t>());
		OT_CORE_API_EXPORT std::vector<int32_t> getIntVector(const JsonValue& _value, const std::string& _member, const std::vector<int32_t>& _default = std::vector<int32_t>());
		OT_CORE_API_EXPORT std::vector<int32_t> getIntVector(const ConstJsonObject& _value, const char* _member, const std::vector<int32_t>& _default = std::vector<int32_t>());
		OT_CORE_API_EXPORT std::vector<int32_t> getIntVector(const ConstJsonObject& _value, const std::string& _member, const std::vector<int32_t>& _default = std::vector<int32_t>());

		//! @brief Check and return the requested value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT std::vector<uint32_t> getUIntVector(const JsonValue& _value, const char* _member, const std::vector<uint32_t>& _default = std::vector<uint32_t>());
		OT_CORE_API_EXPORT std::vector<uint32_t> getUIntVector(const JsonValue& _value, const std::string& _member, const std::vector<uint32_t>& _default = std::vector<uint32_t>());
		OT_CORE_API_EXPORT std::vector<uint32_t> getUIntVector(const ConstJsonObject& _value, const char* _member, const std::vector<uint32_t>& _default = std::vector<uint32_t>());
		OT_CORE_API_EXPORT std::vector<uint32_t> getUIntVector(const ConstJsonObject& _value, const std::string& _member, const std::vector<uint32_t>& _default = std::vector<uint32_t>());

		//! @brief Check and return the requested value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT std::vector<int64_t> getInt64Vector(const JsonValue& _value, const char* _member, const std::vector<int64_t>& _default = std::vector<int64_t>());
		OT_CORE_API_EXPORT std::vector<int64_t> getInt64Vector(const JsonValue& _value, const std::string& _member, const std::vector<int64_t>& _default = std::vector<int64_t>());
		OT_CORE_API_EXPORT std::vector<int64_t> getInt64Vector(const ConstJsonObject& _value, const char* _member, const std::vector<int64_t>& _default = std::vector<int64_t>());
		OT_CORE_API_EXPORT std::vector<int64_t> getInt64Vector(const ConstJsonObject& _value, const std::string& _member, const std::vector<int64_t>& _default = std::vector<int64_t>());

		//! @brief Check and return the requested value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT std::vector<uint64_t> getUInt64Vector(const JsonValue& _value, const char* _member, const std::vector<uint64_t>& _default = std::vector<uint64_t>());
		OT_CORE_API_EXPORT std::vector<uint64_t> getUInt64Vector(const JsonValue& _value, const std::string& _member, const std::vector<uint64_t>& _default = std::vector<uint64_t>());
		OT_CORE_API_EXPORT std::vector<uint64_t> getUInt64Vector(const ConstJsonObject& _value, const char* _member, const std::vector<uint64_t>& _default = std::vector<uint64_t>());
		OT_CORE_API_EXPORT std::vector<uint64_t> getUInt64Vector(const ConstJsonObject& _value, const std::string& _member, const std::vector<uint64_t>& _default = std::vector<uint64_t>());

		//! @brief Check and return the requested value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT std::vector<float> getFloatVector(const JsonValue& _value, const char* _member, const std::vector<float>& _default = std::vector<float>());
		OT_CORE_API_EXPORT std::vector<float> getFloatVector(const JsonValue& _value, const std::string& _member, const std::vector<float>& _default = std::vector<float>());
		OT_CORE_API_EXPORT std::vector<float> getFloatVector(const ConstJsonObject& _value, const char* _member, const std::vector<float>& _default = std::vector<float>());
		OT_CORE_API_EXPORT std::vector<float> getFloatVector(const ConstJsonObject& _value, const std::string& _member, const std::vector<float>& _default = std::vector<float>());

		//! @brief Check and return the requested value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT std::vector<double> getDoubleVector(const JsonValue& _value, const char* _member, const std::vector<double>& _default = std::vector<double>());
		OT_CORE_API_EXPORT std::vector<double> getDoubleVector(const JsonValue& _value, const std::string& _member, const std::vector<double>& _default = std::vector<double>());
		OT_CORE_API_EXPORT std::vector<double> getDoubleVector(const ConstJsonObject& _value, const char* _member, const std::vector<double>& _default = std::vector<double>());
		OT_CORE_API_EXPORT std::vector<double> getDoubleVector(const ConstJsonObject& _value, const std::string& _member, const std::vector<double>& _default = std::vector<double>());

		//! @brief Check and return the requested value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT std::vector<std::string> getStringVector(const JsonValue& _value, const char* _member, const std::vector<std::string>& _default = std::vector<std::string>());
		OT_CORE_API_EXPORT std::vector<std::string> getStringVector(const JsonValue& _value, const std::string& _member, const std::vector<std::string>& _default = std::vector<std::string>());
		OT_CORE_API_EXPORT std::vector<std::string> getStringVector(const ConstJsonObject& _value, const char* _member, const std::vector<std::string>& _default = std::vector<std::string>());
		OT_CORE_API_EXPORT std::vector<std::string> getStringVector(const ConstJsonObject& _value, const std::string& _member, const std::vector<std::string>& _default = std::vector<std::string>());

		//! @brief Check and return the requested value
		//! Asserts, logs and throws when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT std::vector<ConstJsonObject> getObjectVector(const JsonValue& _value, const char* _member);
		OT_CORE_API_EXPORT std::vector<ConstJsonObject> getObjectVector(const JsonValue& _value, const std::string& _member);
		OT_CORE_API_EXPORT std::vector<ConstJsonObject> getObjectVector(const ConstJsonObject& _value, const char* _member);
		OT_CORE_API_EXPORT std::vector<ConstJsonObject> getObjectVector(const ConstJsonObject& _value, const std::string& _member);

		//! @brief Check and return the requested value
		//! Asserts, logs and throws when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT std::vector<ConstJsonArray> getArrayVector(const JsonValue& _value, const char* _member);
		OT_CORE_API_EXPORT std::vector<ConstJsonArray> getArrayVector(const JsonValue& _value, const std::string& _member);
		OT_CORE_API_EXPORT std::vector<ConstJsonArray> getArrayVector(const ConstJsonObject& _value, const char* _member);
		OT_CORE_API_EXPORT std::vector<ConstJsonArray> getArrayVector(const ConstJsonObject& _value, const std::string& _member);

		//! @brief Check and return the requested value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT bool* getBoolArray(const JsonValue& _value, const char* _member, JsonSizeType& _size);
		OT_CORE_API_EXPORT bool* getBoolArray(const JsonValue& _value, const std::string& _member, JsonSizeType& _size);
		OT_CORE_API_EXPORT bool* getBoolArray(const ConstJsonObject& _value, const char* _member, JsonSizeType& _size);
		OT_CORE_API_EXPORT bool* getBoolArray(const ConstJsonObject& _value, const std::string& _member, JsonSizeType& _size);

		//! @brief Check and return the requested value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT int32_t* getIntArray(const JsonValue& _value, const char* _member, JsonSizeType& _size);
		OT_CORE_API_EXPORT int32_t* getIntArray(const JsonValue& _value, const std::string& _member, JsonSizeType& _size);
		OT_CORE_API_EXPORT int32_t* getIntArray(const ConstJsonObject& _value, const char* _member, JsonSizeType& _size);
		OT_CORE_API_EXPORT int32_t* getIntArray(const ConstJsonObject& _value, const std::string& _member, JsonSizeType& _size);

		//! @brief Check and return the requested value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT uint32_t* getUIntArray(const JsonValue& _value, const char* _member, JsonSizeType& _size);
		OT_CORE_API_EXPORT uint32_t* getUIntArray(const JsonValue& _value, const std::string& _member, JsonSizeType& _size);
		OT_CORE_API_EXPORT uint32_t* getUIntArray(const ConstJsonObject& _value, const char* _member, JsonSizeType& _size);
		OT_CORE_API_EXPORT uint32_t* getUIntArray(const ConstJsonObject& _value, const std::string& _member, JsonSizeType& _size);

		//! @brief Check and return the requested value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT int64_t* getInt64Array(const JsonValue& _value, const char* _member, JsonSizeType& _size);
		OT_CORE_API_EXPORT int64_t* getInt64Array(const JsonValue& _value, const std::string& _member, JsonSizeType& _size);
		OT_CORE_API_EXPORT int64_t* getInt64Array(const ConstJsonObject& _value, const char* _member, JsonSizeType& _size);
		OT_CORE_API_EXPORT int64_t* getInt64Array(const ConstJsonObject& _value, const std::string& _member, JsonSizeType& _size);

		//! @brief Check and return the requested value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT uint64_t* getUInt64Array(const JsonValue& _value, const char* _member, JsonSizeType& _size);
		OT_CORE_API_EXPORT uint64_t* getUInt64Array(const JsonValue& _value, const std::string& _member, JsonSizeType& _size);
		OT_CORE_API_EXPORT uint64_t* getUInt64Array(const ConstJsonObject& _value, const char* _member, JsonSizeType& _size);
		OT_CORE_API_EXPORT uint64_t* getUInt64Array(const ConstJsonObject& _value, const std::string& _member, JsonSizeType& _size);

		//! @brief Check and return the requested value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT float* getFloatArray(const JsonValue& _value, const char* _member, JsonSizeType& _size);
		OT_CORE_API_EXPORT float* getFloatArray(const JsonValue& _value, const std::string& _member, JsonSizeType& _size);
		OT_CORE_API_EXPORT float* getFloatArray(const ConstJsonObject& _value, const char* _member, JsonSizeType& _size);
		OT_CORE_API_EXPORT float* getFloatArray(const ConstJsonObject& _value, const std::string& _member, JsonSizeType& _size);

		//! @brief Check and return the requested value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT double* getDoubleArray(const JsonValue& _value, const char* _member, JsonSizeType& _size);
		OT_CORE_API_EXPORT double* getDoubleArray(const JsonValue& _value, const std::string& _member, JsonSizeType& _size);
		OT_CORE_API_EXPORT double* getDoubleArray(const ConstJsonObject& _value, const char* _member, JsonSizeType& _size);
		OT_CORE_API_EXPORT double* getDoubleArray(const ConstJsonObject& _value, const std::string& _member, JsonSizeType& _size);

		// ###########################################################################################################################################################################################################################################################################################################################

		// ###########################################################################################################################################################################################################################################################################################################################

		// ###########################################################################################################################################################################################################################################################################################################################

		//! @brief Returns true if the specified array entry is a null
		OT_CORE_API_EXPORT bool isNull(const JsonValue& _value, unsigned int _ix);
		//! @brief Returns true if the specified array entry is a null
		OT_CORE_API_EXPORT bool isNull(const ConstJsonArray& _value, unsigned int _ix);

		//! @brief Returns true if the specified array entry is a bool
		OT_CORE_API_EXPORT bool isBool(const JsonValue& _value, unsigned int _ix);
		//! @brief Returns true if the specified array entry is a bool
		OT_CORE_API_EXPORT bool isBool(const ConstJsonArray& _value, unsigned int _ix);

		//! @brief Returns true if the specified array entry is a int
		OT_CORE_API_EXPORT bool isInt(const JsonValue& _value, unsigned int _ix);
		//! @brief Returns true if the specified array entry is a int
		OT_CORE_API_EXPORT bool isInt(const ConstJsonArray& _value, unsigned int _ix);

		//! @brief Returns true if the specified array entry is a uInt
		OT_CORE_API_EXPORT bool isUInt(const JsonValue& _value, unsigned int _ix);
		//! @brief Returns true if the specified array entry is a uInt
		OT_CORE_API_EXPORT bool isUInt(const ConstJsonArray& _value, unsigned int _ix);

		//! @brief Returns true if the specified array entry is a int64
		OT_CORE_API_EXPORT bool isInt64(const JsonValue& _value, unsigned int _ix);
		//! @brief Returns true if the specified array entry is a int64
		OT_CORE_API_EXPORT bool isInt64(const ConstJsonArray& _value, unsigned int _ix);

		//! @brief Returns true if the specified array entry is a uInt64
		OT_CORE_API_EXPORT bool isUInt64(const JsonValue& _value, unsigned int _ix);
		//! @brief Returns true if the specified array entry is a uInt64
		OT_CORE_API_EXPORT bool isUInt64(const ConstJsonArray& _value, unsigned int _ix);

		//! @brief Returns true if the specified array entry is a float
		OT_CORE_API_EXPORT bool isFloat(const JsonValue& _value, unsigned int _ix);
		//! @brief Returns true if the specified array entry is a float
		OT_CORE_API_EXPORT bool isFloat(const ConstJsonArray& _value, unsigned int _ix);

		//! @brief Returns true if the specified array entry is a double
		OT_CORE_API_EXPORT bool isDouble(const JsonValue& _value, unsigned int _ix);
		//! @brief Returns true if the specified array entry is a double
		OT_CORE_API_EXPORT bool isDouble(const ConstJsonArray& _value, unsigned int _ix);

		//! @brief Returns true if the specified array entry is a string
		OT_CORE_API_EXPORT bool isString(const JsonValue& _value, unsigned int _ix);
		//! @brief Returns true if the specified array entry is a string
		OT_CORE_API_EXPORT bool isString(const ConstJsonArray& _value, unsigned int _ix);

		//! @brief Returns true if the specified array entry is a object
		OT_CORE_API_EXPORT bool isObject(const JsonValue& _value, unsigned int _ix);
		//! @brief Returns true if the specified array entry is a object
		OT_CORE_API_EXPORT bool isObject(const ConstJsonArray& _value, unsigned int _ix);

		//! @brief Returns true if the specified array entry is a array
		OT_CORE_API_EXPORT bool isArray(const JsonValue& _value, unsigned int _ix);
		//! @brief Returns true if the specified array entry is a array
		OT_CORE_API_EXPORT bool isArray(const ConstJsonArray& _value, unsigned int _ix);

		//! @brief Check and return the requested value from the provided array value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT bool getBool(const JsonValue& _value, unsigned int _ix, bool _default = false);
		OT_CORE_API_EXPORT bool getBool(const ConstJsonArray& _value, unsigned int _ix, bool _default = false);

		//! @brief Check and return the requested value from the provided array value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT int32_t getInt(const JsonValue& _value, unsigned int _ix, int32_t _default = 0);
		OT_CORE_API_EXPORT int32_t getInt(const ConstJsonArray& _value, unsigned int _ix, int32_t _default = 0);

		//! @brief Check and return the requested value from the provided array value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT uint32_t getUInt(const JsonValue& _value, unsigned int _ix, uint32_t _default = 0);
		OT_CORE_API_EXPORT uint32_t getUInt(const ConstJsonArray& _value, unsigned int _ix, uint32_t _default = 0);

		//! @brief Check and return the requested value from the provided array value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT int64_t getInt64(const JsonValue& _value, unsigned int _ix, int64_t _default = 0);
		OT_CORE_API_EXPORT int64_t getInt64(const ConstJsonArray& _value, unsigned int _ix, int64_t _default = 0);

		//! @brief Check and return the requested value from the provided array value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT uint64_t getUInt64(const JsonValue& _value, unsigned int _ix, uint64_t _default = 0);
		OT_CORE_API_EXPORT uint64_t getUInt64(const ConstJsonArray& _value, unsigned int _ix, uint64_t _default = 0);

		//! @brief Check and return the requested value from the provided array value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT float getFloat(const JsonValue& _value, unsigned int _ix, float _default = 0.f);
		OT_CORE_API_EXPORT float getFloat(const ConstJsonArray& _value, unsigned int _ix, float _default = 0.f);

		//! @brief Check and return the requested value from the provided array value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT double getDouble(const JsonValue& _value, unsigned int _ix, double _default = 0.);
		OT_CORE_API_EXPORT double getDouble(const ConstJsonArray& _value, unsigned int _ix, double _default = 0.);

		//! @brief Check and return the requested value from the provided array value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT std::string getString(const JsonValue& _value, unsigned int _ix, const std::string& _default = std::string());
		OT_CORE_API_EXPORT std::string getString(const ConstJsonArray& _value, unsigned int _ix, const std::string& _default = std::string());

		//! @brief Check and return the requested value from the provided array value
		//! Asserts, logs and throws when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT ConstJsonObject getObject(const JsonValue& _value, unsigned int _ix);
		OT_CORE_API_EXPORT ConstJsonObject getObject(const ConstJsonArray& _value, unsigned int _ix);

		//! @brief Check and return the requested value from the provided array value
		//! Asserts, logs and throws when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT ConstJsonArray getArray(const JsonValue& _value, unsigned int _ix);
		OT_CORE_API_EXPORT ConstJsonArray getArray(const ConstJsonArray& _value, unsigned int _ix);

		//! @brief Check and return the requested value from the provided array value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT std::list<int32_t> getIntList(const JsonValue& _value, unsigned int _ix, const std::list<int32_t>& _default = std::list<int32_t>());
		OT_CORE_API_EXPORT std::list<int32_t> getIntList(const ConstJsonArray& _value, unsigned int _ix, const std::list<int32_t>& _default = std::list<int32_t>());

		//! @brief Check and return the requested value from the provided array value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT std::list<uint32_t> getUIntList(const JsonValue& _value, unsigned int _ix, const std::list<uint32_t>& _default = std::list<uint32_t>());
		OT_CORE_API_EXPORT std::list<uint32_t> getUIntList(const ConstJsonArray& _value, unsigned int _ix, const std::list<uint32_t>& _default = std::list<uint32_t>());

		//! @brief Check and return the requested value from the provided array value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT std::list<int64_t> getInt64List(const JsonValue& _value, unsigned int _ix, const std::list<int64_t>& _default = std::list<int64_t>());
		OT_CORE_API_EXPORT std::list<int64_t> getInt64List(const ConstJsonArray& _value, unsigned int _ix, const std::list<int64_t>& _default = std::list<int64_t>());

		//! @brief Check and return the requested value from the provided array value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT std::list<uint64_t> getUInt64List(const JsonValue& _value, unsigned int _ix, const std::list<uint64_t>& _default = std::list<uint64_t>());
		OT_CORE_API_EXPORT std::list<uint64_t> getUInt64List(const ConstJsonArray& _value, unsigned int _ix, const std::list<uint64_t>& _default = std::list<uint64_t>());

		//! @brief Check and return the requested value from the provided array value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT std::list<std::string> getStringList(const JsonValue& _value, unsigned int _ix, const std::list<std::string>& _default = std::list<std::string>());
		OT_CORE_API_EXPORT std::list<std::string> getStringList(const ConstJsonArray& _value, unsigned int _ix, const std::list<std::string>& _default = std::list<std::string>());

		//! @brief Check and return the requested value from the provided array value
		//! Asserts, logs and throws when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT std::list<ConstJsonObject> getObjectList(const JsonValue& _value, unsigned int _ix);
		OT_CORE_API_EXPORT std::list<ConstJsonObject> getObjectList(const ConstJsonArray& _value, unsigned int _ix);

		//! @brief Check and return the requested value from the provided array value
		//! Asserts, logs and throws when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT std::list<ConstJsonArray> getArrayList(const JsonValue& _value, unsigned int _ix);
		OT_CORE_API_EXPORT std::list<ConstJsonArray> getArrayList(const ConstJsonArray& _value, unsigned int _ix);

		//! @brief Check and return the requested value from the provided array value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT std::vector<int32_t> getIntVector(const JsonValue& _value, unsigned int _ix, const std::vector<int32_t>& _default = std::vector<int32_t>());
		OT_CORE_API_EXPORT std::vector<int32_t> getIntVector(const ConstJsonArray& _value, unsigned int _ix, const std::vector<int32_t>& _default = std::vector<int32_t>());

		//! @brief Check and return the requested value from the provided array value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT std::vector<uint32_t> getUIntVector(const JsonValue& _value, unsigned int _ix, const std::vector<uint32_t>& _default = std::vector<uint32_t>());
		OT_CORE_API_EXPORT std::vector<uint32_t> getUIntVector(const ConstJsonArray& _value, unsigned int _ix, const std::vector<uint32_t>& _default = std::vector<uint32_t>());

		//! @brief Check and return the requested value from the provided array value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT std::vector<int64_t> getInt64Vector(const JsonValue& _value, unsigned int _ix, const std::vector<int64_t>& _default = std::vector<int64_t>());
		OT_CORE_API_EXPORT std::vector<int64_t> getInt64Vector(const ConstJsonArray& _value, unsigned int _ix, const std::vector<int64_t>& _default = std::vector<int64_t>());

		//! @brief Check and return the requested value from the provided array value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT std::vector<uint64_t> getUInt64Vector(const JsonValue& _value, unsigned int _ix, const std::vector<uint64_t>& _default = std::vector<uint64_t>());
		OT_CORE_API_EXPORT std::vector<uint64_t> getUInt64Vector(const ConstJsonArray& _value, unsigned int _ix, const std::vector<uint64_t>& _default = std::vector<uint64_t>());

		//! @brief Check and return the requested value from the provided array value
		//! Asserts, logs and returns default when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT std::vector<std::string> getStringVector(const JsonValue& _value, unsigned int _ix, const std::vector<std::string>& _default = std::vector<std::string>());
		OT_CORE_API_EXPORT std::vector<std::string> getStringVector(const ConstJsonArray& _value, unsigned int _ix, const std::vector<std::string>& _default = std::vector<std::string>());

		//! @brief Check and return the requested value from the provided array value
		//! Asserts, logs and throws when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT std::vector<ConstJsonObject> getObjectVector(const JsonValue& _value, unsigned int _ix);
		OT_CORE_API_EXPORT std::vector<ConstJsonObject> getObjectVector(const ConstJsonArray& _value, unsigned int _ix);

		//! @brief Check and return the requested value from the provided array value
		//! Asserts, logs and throws when the provided member does not exist or is invalid type
		OT_CORE_API_EXPORT std::vector<ConstJsonArray> getArrayVector(const JsonValue& _value, unsigned int _ix);
		OT_CORE_API_EXPORT std::vector<ConstJsonArray> getArrayVector(const ConstJsonArray& _value, unsigned int _ix);

		//! @brief Returns the serialized json value.
		OT_CORE_API_EXPORT std::string toJson(const JsonValue& _obj);

		//! @brief Returns the serialized json object.
		OT_CORE_API_EXPORT std::string toJson(const JsonObject& _obj);

		//! @brief Returns the serialized json object.
		OT_CORE_API_EXPORT std::string toJson(const ConstJsonObject& _obj);

		//! @brief Returns the serialized json array.
		OT_CORE_API_EXPORT std::string toJson(const JsonArray& _arr);

		//! @brief Returns the serialized json array.
		OT_CORE_API_EXPORT std::string toJson(const ConstJsonArray& _arr);
		
		
		OT_CORE_API_EXPORT std::string toPrettyString(JsonValue& _value);

	} // namespace json

}