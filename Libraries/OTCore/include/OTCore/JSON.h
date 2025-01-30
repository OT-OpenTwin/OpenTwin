//! @file JSON.h
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/CoreAPIExport.h"
#include "OTCore/OTClassHelper.h"

// rapidjson header
#include <rapidjson/document.h>
#include <rapidjson/writer.h>

// std header
#include <list>
#include <vector>
#include <string>

namespace ot {

	//! @brief Size type
	typedef rapidjson::SizeType JsonSizeType;

	//! @brief Writable JSON value
	typedef rapidjson::Value JsonValue;

	//! @brief Allocator used for writing to JSON values
	typedef rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> JsonAllocator;

	//typedef rapidjson::GenericObject<false, rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>>> JsonObjectHandle;
	
	//! @brief Read only JSON Object
	typedef rapidjson::GenericObject<true, rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>>> ConstJsonObject;
	
	typedef std::list<ConstJsonObject> ConstJsonObjectList;
	
	//typedef rapidjson::GenericArray<false, rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>>> JsonArrayHandle;
	
	//! @brief Read only JSON Array
	typedef rapidjson::GenericArray<true, rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>>> ConstJsonArray;
	
	typedef std::list<ConstJsonArray> ConstJsonArrayList;

	//! @brief Read only JSON Object iterator
	typedef rapidjson::GenericMemberIterator<true, rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>> JsonMemberIterator;

	//! @class JsonNullValue
	//! @brief JSON NULL value
	class JsonNullValue : public JsonValue {
		OT_DECL_NOCOPY(JsonNullValue)
	public:
		JsonNullValue() : JsonValue(rapidjson::kNullType) {};
		virtual ~JsonNullValue() {};
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class JsonNumber : public JsonValue {
		OT_DECL_NOCOPY(JsonNumber)
		OT_DECL_NODEFAULT(JsonNumber)
	public:
		JsonNumber(int _value) : JsonValue(rapidjson::kNumberType) { this->SetInt64(static_cast<long>(_value)); };
		JsonNumber(long _value) : JsonValue(rapidjson::kNumberType) { this->SetInt64(_value); };
		JsonNumber(long long _value) : JsonValue(rapidjson::kNumberType) { this->SetInt64(_value); };
		JsonNumber(unsigned long _value) : JsonValue(rapidjson::kNumberType) { this->SetUint64(_value); };
		JsonNumber(unsigned long long _value) : JsonValue(rapidjson::kNumberType) { this->SetUint64(_value); };
		JsonNumber(float _value) : JsonValue(rapidjson::kNumberType) { this->SetFloat(_value); };
		JsonNumber(double _value) : JsonValue(rapidjson::kNumberType) { this->SetDouble(_value); };
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	//! @class JsonString
	//! @brief JSON String value
	class JsonString : public JsonValue {
		OT_DECL_NOCOPY(JsonString)
		OT_DECL_NODEFAULT(JsonString)
	public:
		JsonString(const char* _cstr, JsonAllocator& _allocator) : JsonValue(_cstr, _allocator) {};
		JsonString(const std::string& _str, JsonAllocator& _allocator) : JsonValue(_str.c_str(), _allocator) {};
		virtual ~JsonString() {};
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	//! @class JsonObject
	//! @brief JSON Object value
	class JsonObject : public JsonValue {
		OT_DECL_NOCOPY(JsonObject)
	public:
		JsonObject() : JsonValue(rapidjson::kObjectType) {};
		virtual ~JsonObject() {};
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################
	 
	//! @class JsonArray
	//! @brief JSON Array value
	class JsonArray : public JsonValue {
		OT_DECL_NOCOPY(JsonArray)
	public:
		explicit JsonArray() : JsonValue(rapidjson::kArrayType) {};

		//! @brief Create bool array
		JsonArray(const std::list<bool>& _lst, JsonAllocator& _allocator)
			: rapidjson::Value(rapidjson::kArrayType)
		{
			for (auto itm : _lst) {
				this->PushBack(itm, _allocator);
			}
		}

		//! @brief Create bool array
		explicit JsonArray(const std::vector<bool>& _vec, JsonAllocator& _allocator)
			: rapidjson::Value(rapidjson::kArrayType)
		{
			for (auto itm : _vec) {
				this->PushBack(itm, _allocator);
			}
		}

		//! @brief Create int32 array
		JsonArray(const std::list<int32_t>& _lst, JsonAllocator& _allocator)
			: rapidjson::Value(rapidjson::kArrayType)
		{
			for (auto itm : _lst) {
				this->PushBack(itm, _allocator);
			}
		}

		//! @brief Create int32 array
		explicit JsonArray(const std::vector<int32_t>& _vec, JsonAllocator& _allocator)
			: rapidjson::Value(rapidjson::kArrayType)
		{
			for (auto itm : _vec) {
				this->PushBack(itm, _allocator);
			}
		}

		//! @brief Create uint32 array
		explicit JsonArray(const std::list<uint32_t>& _lst, JsonAllocator& _allocator)
			: rapidjson::Value(rapidjson::kArrayType)
		{
			for (auto itm : _lst) {
				this->PushBack(itm, _allocator);
			}
		}

		//! @brief Create uint32 array
		explicit JsonArray(const std::vector<uint32_t>& _vec, JsonAllocator& _allocator)
			: rapidjson::Value(rapidjson::kArrayType)
		{
			for (auto itm : _vec) {
				this->PushBack(itm, _allocator);
			}
		}

		//! @brief Create int64 array
		explicit JsonArray(const std::list<int64_t>& _lst, JsonAllocator& _allocator)
			: rapidjson::Value(rapidjson::kArrayType)
		{
			for (auto itm : _lst) {
				this->PushBack(itm, _allocator);
			}
		}

		//! @brief Create int64 array
		explicit JsonArray(const std::vector<int64_t>& _vec, JsonAllocator& _allocator)
			: rapidjson::Value(rapidjson::kArrayType)
		{
			for (auto itm : _vec) {
				this->PushBack(itm, _allocator);
			}
		}

		//! @brief Create uint64 array
		explicit JsonArray(const std::list<uint64_t>& _lst, JsonAllocator& _allocator)
			: rapidjson::Value(rapidjson::kArrayType)
		{
			for (auto itm : _lst) {
				this->PushBack(itm, _allocator);
			}
		}

		//! @brief Create uint64 array
		explicit JsonArray(const std::vector<uint64_t>& _vec, JsonAllocator& _allocator)
			: rapidjson::Value(rapidjson::kArrayType)
		{
			for (auto itm : _vec) {
				this->PushBack(itm, _allocator);
			}
		}

		//! @brief Create float array
		explicit JsonArray(const std::list<float>& _lst, JsonAllocator& _allocator)
			: rapidjson::Value(rapidjson::kArrayType)
		{
			for (auto itm : _lst) {
				this->PushBack(itm, _allocator);
			}
		}

		//! @brief Create float array
		explicit JsonArray(const std::vector<float>& _vec, JsonAllocator& _allocator)
			: rapidjson::Value(rapidjson::kArrayType)
		{
			for (auto itm : _vec) {
				this->PushBack(itm, _allocator);
			}
		}

		//! @brief Create double array
		explicit JsonArray(const std::list<double>& _lst, JsonAllocator& _allocator)
			: rapidjson::Value(rapidjson::kArrayType)
		{
			for (auto itm : _lst) {
				this->PushBack(itm, _allocator);
			}
		}

		//! @brief Create double array
		explicit JsonArray(const std::vector<double>& _vec, JsonAllocator& _allocator)
			: rapidjson::Value(rapidjson::kArrayType)
		{
			for (auto itm : _vec) {
				this->PushBack(itm, _allocator);
			}
		}

		//! @brief Create string array
		explicit JsonArray(const std::list<std::string>& _lst, JsonAllocator& _allocator)
			: rapidjson::Value(rapidjson::kArrayType)
		{
			for (auto itm : _lst) {
				this->PushBack(JsonString(itm, _allocator), _allocator);
			}
		}

		//! @brief Create string array
		explicit JsonArray(const std::vector<std::string>& _vec, JsonAllocator& _allocator)
			: rapidjson::Value(rapidjson::kArrayType)
		{
			for (auto itm : _vec) {
				this->PushBack(JsonString(itm, _allocator), _allocator);
			}
		}
		
		virtual ~JsonArray() {};
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	//! @class JsonDocument
	//! @brief JSON document
	class JsonDocument : public rapidjson::Document {
	private:
		JsonDocument(const JsonDocument&) = delete;
		JsonDocument& operator = (const JsonDocument&) = delete;
	public:
		//! @brief Constructor
		//! Set the document as object
		JsonDocument() : rapidjson::Document(rapidjson::kObjectType) {};

		//! @brief Constructor
		//! @param _type The type for the json document
		JsonDocument(rapidjson::Type _type) : rapidjson::Document(_type) {};

		//! @brief Move constructor
		JsonDocument(JsonDocument&& _other) noexcept : rapidjson::Document((rapidjson::Document&& )_other) {};

		virtual ~JsonDocument() {};

		bool fromJson(const std::string& _json) { return this->fromJson(_json.c_str()); };
		bool fromJson(const char* _fromJson) {
			this->Parse(_fromJson);
			return (this->GetType() == rapidjson::kObjectType) || (this->GetType() == rapidjson::kArrayType);
		}

		const JsonDocument& constRef(void) const { return *this; };

		ConstJsonObject GetConstObject() const {
			return this->GetObject();
		}

		std::string toJson(void) const {
			// Create String buffer
			rapidjson::StringBuffer buffer;
			buffer.Clear();

			// Setup the Writer
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			this->Accept(writer);

			// Return string
			return std::string(buffer.GetString());
		}
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

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

	} // namespace json
}
