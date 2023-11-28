//! @file JSON.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/Logger.h"

#define OT_JSON_checkMemberExists(___object, ___memberName, ___errorAction) if (!___object.HasMember(___memberName)) { OT_LOG_EAS("JSON object member \"" + std::string(___memberName) + "\" missing"); ___errorAction; }

#define OT_JSON_createMemberIterator(___object, ___memberName, ___iteratorName) ot::JsonMemberIterator ___iteratorName = ___object.FindMember(___memberName)

#define OT_JSON_checkMemberIteratorType(___it, ___memberName, ___memberType, ___errorAction) if (!___it->value.Is##___memberType()) { OT_LOG_EAS("JSON object member \"" + std::string(___memberName) + "\" is not a " #___memberType); ___errorAction; }

#define OT_JSON_getFromObject(___object, ___memberName, ___memberType, ___errorAction) \
OT_JSON_checkMemberExists(___object, ___memberName, ___errorAction); \
OT_JSON_createMemberIterator(___object, ___memberName, lclit); \
OT_JSON_checkMemberIteratorType(lclit, ___memberName, ___memberType, ___errorAction); \
return lclit->value.Get##___memberType();

#define OT_JSON_getListFromObject(___object, ___memberName, ___entryType, ___list, ___errorAction) \
OT_JSON_checkMemberExists(___object, ___memberName, ___errorAction); \
OT_JSON_createMemberIterator(___object, ___memberName, lclit); \
OT_JSON_checkMemberIteratorType(lclit, ___memberName, Array, ___errorAction); \
for (rapidjson::SizeType i = 0; i < lclit->value.Size(); i++) { \
	if (!lclit->value[i].Is##___entryType()) { OT_LOG_EAS("Array entry at index \"" + std::to_string(i) + "\" is not a " #___entryType); ___errorAction; } \
	___list.push_back(lclit->value[i].Get##___entryType()); \
} \
return ___list

#define OT_JSON_checkArrayEntry(___array, ___ix, ___entryType, ___errorAction) if (!_value[_ix].Is##___entryType()) { OT_LOG_EAS("Array entry \"" + std::to_string(_ix) + "\" is not a " #___entryType); ___errorAction; }

#define OT_JSON_getFromArray(___array, ___ix, ___entryType, ___errorAction) \
OT_JSON_checkArrayEntry(___array, ___ix, ___entryType, ___errorAction); \
return ___array[___ix].Get##___entryType();

#define OT_JSON_getListFromArray(___array, ___ix, ___entryType, ___list, ___errorAction) \
OT_JSON_checkArrayEntry(___array, ___ix, Array, ___errorAction); \
ot::ConstJsonArray it = ___array[___ix].Get##Array(); \
for (rapidjson::SizeType i = 0; i < it.Size(); i++) { \
	___list.push_back(it[i].Get##___entryType()); \
} \
return ___list;


bool ot::json::getBool(const JsonValue& _value, const char* _member, bool _default) {
	OT_JSON_getFromObject(_value, _member, Bool, return _default);
}

bool ot::json::getBool(const ConstJsonObject& _value, const char* _member, bool _default) {
	OT_JSON_getFromObject(_value, _member, Bool, return _default);
}

int32_t ot::json::getInt(const JsonValue& _value, const char* _member, int32_t _default) {
	OT_JSON_getFromObject(_value, _member, Int, return _default);
}

int32_t ot::json::getInt(const ConstJsonObject& _value, const char* _member, int32_t _default) {
	OT_JSON_getFromObject(_value, _member, Int, return _default);
}

uint32_t ot::json::getUInt(const JsonValue& _value, const char* _member, uint32_t _default) {
	OT_JSON_getFromObject(_value, _member, Uint, return _default);
}

uint32_t ot::json::getUInt(const ConstJsonObject& _value, const char* _member, uint32_t _default) {
	OT_JSON_getFromObject(_value, _member, Uint, return _default);
}

int64_t ot::json::getInt64(const JsonValue& _value, const char* _member, int64_t _default) {
	OT_JSON_getFromObject(_value, _member, Int64, return _default);
}

int64_t ot::json::getInt64(const ConstJsonObject& _value, const char* _member, int64_t _default) {
	OT_JSON_getFromObject(_value, _member, Int64, return _default);
}

uint64_t ot::json::getUInt64(const JsonValue& _value, const char* _member, uint64_t _default) {
	OT_JSON_getFromObject(_value, _member, Uint64, return _default);
}

uint64_t ot::json::getUInt64(const ConstJsonObject& _value, const char* _member, uint64_t _default) {
	OT_JSON_getFromObject(_value, _member, Uint64, return _default);
}

float ot::json::getFloat(const JsonValue& _value, const char* _member, float _default) {
	OT_JSON_getFromObject(_value, _member, Float, return _default);
}

float ot::json::getFloat(const ConstJsonObject& _value, const char* _member, float _default) {
	OT_JSON_getFromObject(_value, _member, Float, return _default);
}

double ot::json::getDouble(const JsonValue& _value, const char* _member, double _default) {
	OT_JSON_getFromObject(_value, _member, Double, return _default);
}

double ot::json::getDouble(const ConstJsonObject& _value, const char* _member, double _default) {
	OT_JSON_getFromObject(_value, _member, Double, return _default);
}

std::string ot::json::getString(const JsonValue& _value, const char* _member, const std::string& _default) {
	OT_JSON_getFromObject(_value, _member, String, return _default);
}

std::string ot::json::getString(const ConstJsonObject& _value, const char* _member, const std::string& _default) {
	OT_JSON_getFromObject(_value, _member, String, return _default);
}

ot::ConstJsonObject ot::json::getObject(const JsonValue& _value, const char* _member) {
	OT_JSON_getFromObject(_value, _member, Object, throw std::exception("Invalid type"));
}

ot::ConstJsonObject ot::json::getObject(const ConstJsonObject& _value, const char* _member) {
	OT_JSON_getFromObject(_value, _member, Object, throw std::exception("Invalid type"));
}

ot::ConstJsonArray ot::json::getArray(const JsonValue& _value, const char* _member) {
	OT_JSON_getFromObject(_value, _member, Array, throw std::exception("Invalid type"));
}

ot::ConstJsonArray ot::json::getArray(const ConstJsonObject& _value, const char* _member) {
	OT_JSON_getFromObject(_value, _member, Array, throw std::exception("Invalid type"));
}

std::list<int32_t> ot::json::getIntList(const JsonValue& _value, const char* _member, const std::list<int32_t>& _default) {
	std::list<int32_t> ret;
	OT_JSON_getListFromObject(_value, _member, Int, ret, return _default);
}

std::list<int32_t> ot::json::getIntList(const ConstJsonObject& _value, const char* _member, const std::list<int32_t>& _default) {
	std::list<int32_t> ret;
	OT_JSON_getListFromObject(_value, _member, Int, ret, return _default);
}

std::list<uint32_t> ot::json::getUIntList(const JsonValue& _value, const char* _member, const std::list<uint32_t>& _default) {
	std::list<uint32_t> ret;
	OT_JSON_getListFromObject(_value, _member, Uint, ret, return _default);
}

std::list<uint32_t> ot::json::getUIntList(const ConstJsonObject& _value, const char* _member, const std::list<uint32_t>& _default) {
	std::list<uint32_t> ret;
	OT_JSON_getListFromObject(_value, _member, Uint, ret, return _default);
}

std::list<int64_t> ot::json::getInt64List(const JsonValue& _value, const char* _member, const std::list<int64_t>& _default) {
	std::list<int64_t> ret;
	OT_JSON_getListFromObject(_value, _member, Int64, ret, return _default);
}

std::list<int64_t> ot::json::getInt64List(const ConstJsonObject& _value, const char* _member, const std::list<int64_t>& _default) {
	std::list<int64_t> ret;
	OT_JSON_getListFromObject(_value, _member, Int64, ret, return _default);
}

std::list<uint64_t> ot::json::getUInt64List(const JsonValue& _value, const char* _member, const std::list<uint64_t>& _default) {
	std::list<uint64_t> ret;
	OT_JSON_getListFromObject(_value, _member, Uint64, ret, return _default);
}

std::list<uint64_t> ot::json::getUInt64List(const ConstJsonObject& _value, const char* _member, const std::list<uint64_t>& _default) {
	std::list<uint64_t> ret;
	OT_JSON_getListFromObject(_value, _member, Uint64, ret, return _default);
}

std::list<float> ot::json::getFloatList(const JsonValue& _value, const char* _member, const std::list<float>& _default) {
	std::list<float> ret;
	OT_JSON_getListFromObject(_value, _member, Float, ret, return _default);
}

std::list<float> ot::json::getFloatList(const ConstJsonObject& _value, const char* _member, const std::list<float>& _default) {
	std::list<float> ret;
	OT_JSON_getListFromObject(_value, _member, Float, ret, return _default);
}

std::list<double> ot::json::getDoubleList(const JsonValue& _value, const char* _member, const std::list<double>& _default) {
	std::list<double> ret;
	OT_JSON_getListFromObject(_value, _member, Double, ret, return _default);
}

std::list<double> ot::json::getDoubleList(const ConstJsonObject& _value, const char* _member, const std::list<double>& _default) {
	std::list<double> ret;
	OT_JSON_getListFromObject(_value, _member, Double, ret, return _default);
}

std::list<std::string> ot::json::getStringList(const JsonValue& _value, const char* _member, const std::list<std::string>& _default) {
	std::list<std::string> ret;
	OT_JSON_getListFromObject(_value, _member, String, ret, return _default);
}

std::list<std::string> ot::json::getStringList(const ConstJsonObject& _value, const char* _member, const std::list<std::string>& _default) {
	std::list<std::string> ret;
	OT_JSON_getListFromObject(_value, _member, String, ret, return _default);
}

std::list<ot::ConstJsonObject> ot::json::getObjectList(const JsonValue& _value, const char* _member) {
	std::list<ot::ConstJsonObject> ret;
	OT_JSON_getListFromObject(_value, _member, Object, ret, throw std::exception("Invalid type"));
}

std::list<ot::ConstJsonObject> ot::json::getObjectList(const ConstJsonObject& _value, const char* _member) {
	std::list<ot::ConstJsonObject> ret;
	OT_JSON_getListFromObject(_value, _member, Object, ret, throw std::exception("Invalid type"));
}

std::list<ot::ConstJsonArray> ot::json::getArrayList(const JsonValue& _value, const char* _member) {
	std::list<ot::ConstJsonArray> ret;
	OT_JSON_getListFromObject(_value, _member, Array, ret, throw std::exception("Invalid type"));
}

std::list<ot::ConstJsonArray> ot::json::getArrayList(const ConstJsonObject& _value, const char* _member) {
	std::list<ot::ConstJsonArray> ret;
	OT_JSON_getListFromObject(_value, _member, Array, ret, throw std::exception("Invalid type"));
}

std::vector<int32_t> ot::json::getIntVector(const JsonValue& _value, const char* _member, const std::vector<int32_t>& _default) {
	std::vector<int32_t> ret;
	OT_JSON_getListFromObject(_value, _member, Int, ret, return _default);
}

std::vector<int32_t> ot::json::getIntVector(const ConstJsonObject& _value, const char* _member, const std::vector<int32_t>& _default) {
	std::vector<int32_t> ret;
	OT_JSON_getListFromObject(_value, _member, Int, ret, return _default);
}

std::vector<uint32_t> ot::json::getUIntVector(const JsonValue& _value, const char* _member, const std::vector<uint32_t>& _default) {
	std::vector<uint32_t> ret;
	OT_JSON_getListFromObject(_value, _member, Uint, ret, return _default);
}

std::vector<uint32_t> ot::json::getUIntVector(const ConstJsonObject& _value, const char* _member, const std::vector<uint32_t>& _default) {
	std::vector<uint32_t> ret;
	OT_JSON_getListFromObject(_value, _member, Uint, ret, return _default);
}

std::vector<int64_t> ot::json::getInt64Vector(const JsonValue& _value, const char* _member, const std::vector<int64_t>& _default) {
	std::vector<int64_t> ret;
	OT_JSON_getListFromObject(_value, _member, Int64, ret, return _default);
}

std::vector<int64_t> ot::json::getInt64Vector(const ConstJsonObject& _value, const char* _member, const std::vector<int64_t>& _default) {
	std::vector<int64_t> ret;
	OT_JSON_getListFromObject(_value, _member, Int64, ret, return _default);
}

std::vector<uint64_t> ot::json::getUInt64Vector(const JsonValue& _value, const char* _member, const std::vector<uint64_t>& _default) {
	std::vector<uint64_t> ret;
	OT_JSON_getListFromObject(_value, _member, Uint64, ret, return _default);
}

std::vector<uint64_t> ot::json::getUInt64Vector(const ConstJsonObject& _value, const char* _member, const std::vector<uint64_t>& _default) {
	std::vector<uint64_t> ret;
	OT_JSON_getListFromObject(_value, _member, Uint64, ret, return _default);
}

std::vector<float> ot::json::getFloatVector(const JsonValue& _value, const char* _member, const std::vector<float>& _default) {
	std::vector<float> ret;
	OT_JSON_getListFromObject(_value, _member, Float, ret, return _default);
}

std::vector<float> ot::json::getFloatVector(const ConstJsonObject& _value, const char* _member, const std::vector<float>& _default) {
	std::vector<float> ret;
	OT_JSON_getListFromObject(_value, _member, Float, ret, return _default);
}

std::vector<double> ot::json::getDoubleVector(const JsonValue& _value, const char* _member, const std::vector<double>& _default) {
	std::vector<double> ret;
	OT_JSON_getListFromObject(_value, _member, Double, ret, return _default);
}

std::vector<double> ot::json::getDoubleVector(const ConstJsonObject& _value, const char* _member, const std::vector<double>& _default) {
	std::vector<double> ret;
	OT_JSON_getListFromObject(_value, _member, Double, ret, return _default);
}

std::vector<std::string> ot::json::getStringVector(const JsonValue& _value, const char* _member, const std::vector<std::string>& _default) {
	std::vector<std::string> ret;
	OT_JSON_getListFromObject(_value, _member, String, ret, return _default);
}

std::vector<std::string> ot::json::getStringVector(const ConstJsonObject& _value, const char* _member, const std::vector<std::string>& _default) {
	std::vector<std::string> ret;
	OT_JSON_getListFromObject(_value, _member, String, ret, return _default);
}

std::vector<ot::ConstJsonObject> ot::json::getObjectVector(const JsonValue& _value, const char* _member) {
	std::vector<ot::ConstJsonObject> ret;
	OT_JSON_getListFromObject(_value, _member, Object, ret, throw std::exception("Invalid type"));
}

std::vector<ot::ConstJsonObject> ot::json::getObjectVector(const ConstJsonObject& _value, const char* _member) {
	std::vector<ot::ConstJsonObject> ret;
	OT_JSON_getListFromObject(_value, _member, Object, ret, throw std::exception("Invalid type"));
}

std::vector<ot::ConstJsonArray> ot::json::getArrayVector(const JsonValue& _value, const char* _member) {
	std::vector<ot::ConstJsonArray> ret;
	OT_JSON_getListFromObject(_value, _member, Array, ret, throw std::exception("Invalid type"));
}

std::vector<ot::ConstJsonArray> ot::json::getArrayVector(const ConstJsonObject& _value, const char* _member) {
	std::vector<ot::ConstJsonArray> ret;
	OT_JSON_getListFromObject(_value, _member, Array, ret, throw std::exception("Invalid type"));
}






bool ot::json::getBool(const JsonValue& _value, unsigned int _ix, bool _default) {
	OT_JSON_getFromArray(_value, _ix, Bool, return _default);
}

bool ot::json::getBool(const ConstJsonArray& _value, unsigned int _ix, bool _default) {
	OT_JSON_getFromArray(_value, _ix, Bool, return _default);
}

int32_t ot::json::getInt(const JsonValue& _value, unsigned int _ix, int32_t _default) {
	OT_JSON_getFromArray(_value, _ix, Int, return _default);
}

int32_t ot::json::getInt(const ConstJsonArray& _value, unsigned int _ix, int32_t _default) {
	OT_JSON_getFromArray(_value, _ix, Int, return _default);
}

uint32_t ot::json::getUInt(const JsonValue& _value, unsigned int _ix, uint32_t _default) {
	OT_JSON_getFromArray(_value, _ix, Uint, return _default);
}

uint32_t ot::json::getUInt(const ConstJsonArray& _value, unsigned int _ix, uint32_t _default) {
	OT_JSON_getFromArray(_value, _ix, Uint, return _default);
}

int64_t ot::json::getInt64(const JsonValue& _value, unsigned int _ix, int64_t _default) {
	OT_JSON_getFromArray(_value, _ix, Int64, return _default);
}

int64_t ot::json::getInt64(const ConstJsonArray& _value, unsigned int _ix, int64_t _default) {
	OT_JSON_getFromArray(_value, _ix, Int64, return _default);
}

uint64_t ot::json::getUInt64(const JsonValue& _value, unsigned int _ix, uint64_t _default) {
	OT_JSON_getFromArray(_value, _ix, Uint64, return _default);
}

uint64_t ot::json::getUInt64(const ConstJsonArray& _value, unsigned int _ix, uint64_t _default) {
	OT_JSON_getFromArray(_value, _ix, Uint64, return _default);
}

float ot::json::getFloat(const JsonValue& _value, unsigned int _ix, float _default) {
	OT_JSON_getFromArray(_value, _ix, Float, return _default);
}

float ot::json::getFloat(const ConstJsonArray& _value, unsigned int _ix, float _default) {
	OT_JSON_getFromArray(_value, _ix, Float, return _default);
}

double ot::json::getDouble(const JsonValue& _value, unsigned int _ix, double _default) {
	OT_JSON_getFromArray(_value, _ix, Double, return _default);
}

double ot::json::getDouble(const ConstJsonArray& _value, unsigned int _ix, double _default) {
	OT_JSON_getFromArray(_value, _ix, Double, return _default);
}

std::string ot::json::getString(const JsonValue& _value, unsigned int _ix, const std::string& _default) {
	OT_JSON_getFromArray(_value, _ix, String, return _default);
}

std::string ot::json::getString(const ConstJsonArray& _value, unsigned int _ix, const std::string& _default) {
	OT_JSON_getFromArray(_value, _ix, String, return _default);
}

ot::ConstJsonObject ot::json::getObject(const JsonValue& _value, unsigned int _ix) {
	OT_JSON_getFromArray(_value, _ix, Object, throw std::exception("Invalid type"));
}

ot::ConstJsonObject ot::json::getObject(const ConstJsonArray& _value, unsigned int _ix) {
	OT_JSON_getFromArray(_value, _ix, Object, throw std::exception("Invalid type"));
}

ot::ConstJsonArray ot::json::getArray(const JsonValue& _value, unsigned int _ix) {
	OT_JSON_getFromArray(_value, _ix, Array, throw std::exception("Invalid type"));
}

ot::ConstJsonArray ot::json::getArray(const ConstJsonArray& _value, unsigned int _ix) {
	OT_JSON_getFromArray(_value, _ix, Array, throw std::exception("Invalid type"));
}

std::list<int32_t> ot::json::getIntList(const JsonValue& _value, unsigned int _ix, const std::list<int32_t>& _default) {
	std::list<int32_t> ret;
	OT_JSON_getListFromArray(_value, _ix, Int, ret, return _default);
}

std::list<int32_t> ot::json::getIntList(const ConstJsonArray& _value, unsigned int _ix, const std::list<int32_t>& _default) {
	std::list<int32_t> ret;
	OT_JSON_getListFromArray(_value, _ix, Int, ret, return _default);
}

std::list<uint32_t> ot::json::getUIntList(const JsonValue& _value, unsigned int _ix, const std::list<uint32_t>& _default) {
	std::list<uint32_t> ret;
	OT_JSON_getListFromArray(_value, _ix, Uint, ret, return _default);
}

std::list<uint32_t> ot::json::getUIntList(const ConstJsonArray& _value, unsigned int _ix, const std::list<uint32_t>& _default) {
	std::list<uint32_t> ret;
	OT_JSON_getListFromArray(_value, _ix, Uint, ret, return _default);
}

std::list<int64_t> ot::json::getInt64List(const JsonValue& _value, unsigned int _ix, const std::list<int64_t>& _default) {
	std::list<int64_t> ret;
	OT_JSON_getListFromArray(_value, _ix, Int64, ret, return _default);
}

std::list<int64_t> ot::json::getInt64List(const ConstJsonArray& _value, unsigned int _ix, const std::list<int64_t>& _default) {
	std::list<int64_t> ret;
	OT_JSON_getListFromArray(_value, _ix, Int64, ret, return _default);
}

std::list<uint64_t> ot::json::getUInt64List(const JsonValue& _value, unsigned int _ix, const std::list<uint64_t>& _default) {
	std::list<uint64_t> ret;
	OT_JSON_getListFromArray(_value, _ix, Uint64, ret, return _default);
}

std::list<uint64_t> ot::json::getUInt64List(const ConstJsonArray& _value, unsigned int _ix, const std::list<uint64_t>& _default) {
	std::list<uint64_t> ret;
	OT_JSON_getListFromArray(_value, _ix, Uint64, ret, return _default);
}

std::list<std::string> ot::json::getStringList(const JsonValue& _value, unsigned int _ix, const std::list<std::string>& _default) {
	std::list<std::string> ret;
	OT_JSON_getListFromArray(_value, _ix, String, ret, return _default);
}

std::list<std::string> ot::json::getStringList(const ConstJsonArray& _value, unsigned int _ix, const std::list<std::string>& _default) {
	std::list<std::string> ret;
	OT_JSON_getListFromArray(_value, _ix, String, ret, return _default);
}

std::list<ot::ConstJsonObject> ot::json::getObjectList(const JsonValue& _value, unsigned int _ix) {
	std::list<ot::ConstJsonObject> ret;
	OT_JSON_getListFromArray(_value, _ix, Object, ret, throw std::exception("Invalid type"));
}

std::list<ot::ConstJsonObject> ot::json::getObjectList(const ConstJsonArray& _value, unsigned int _ix) {
	std::list<ot::ConstJsonObject> ret;
	OT_JSON_getListFromArray(_value, _ix, Object, ret, throw std::exception("Invalid type"));
}

std::list<ot::ConstJsonArray> ot::json::getArrayList(const JsonValue& _value, unsigned int _ix) {
	std::list<ot::ConstJsonArray> ret;
	OT_JSON_getListFromArray(_value, _ix, Array, ret, throw std::exception("Invalid type"));
}

std::list<ot::ConstJsonArray> ot::json::getArrayList(const ConstJsonArray& _value, unsigned int _ix) {
	std::list<ot::ConstJsonArray> ret;
	OT_JSON_getListFromArray(_value, _ix, Array, ret, throw std::exception("Invalid type"));
}

std::vector<int32_t> ot::json::getIntVector(const JsonValue& _value, unsigned int _ix, const std::vector<int32_t>& _default) {
	std::vector<int32_t> ret;
	OT_JSON_getListFromArray(_value, _ix, Int, ret, return _default);
}

std::vector<int32_t> ot::json::getIntVector(const ConstJsonArray& _value, unsigned int _ix, const std::vector<int32_t>& _default) {
	std::vector<int32_t> ret;
	OT_JSON_getListFromArray(_value, _ix, Int, ret, return _default);
}

std::vector<uint32_t> ot::json::getUIntVector(const JsonValue& _value, unsigned int _ix, const std::vector<uint32_t>& _default) {
	std::vector<uint32_t> ret;
	OT_JSON_getListFromArray(_value, _ix, Uint, ret, return _default);
}

std::vector<uint32_t> ot::json::getUIntVector(const ConstJsonArray& _value, unsigned int _ix, const std::vector<uint32_t>& _default) {
	std::vector<uint32_t> ret;
	OT_JSON_getListFromArray(_value, _ix, Uint, ret, return _default);
}

std::vector<int64_t> ot::json::getInt64Vector(const JsonValue& _value, unsigned int _ix, const std::vector<int64_t>& _default) {
	std::vector<int64_t> ret;
	OT_JSON_getListFromArray(_value, _ix, Int64, ret, return _default);
}

std::vector<int64_t> ot::json::getInt64Vector(const ConstJsonArray& _value, unsigned int _ix, const std::vector<int64_t>& _default) {
	std::vector<int64_t> ret;
	OT_JSON_getListFromArray(_value, _ix, Int64, ret, return _default);
}

std::vector<uint64_t> ot::json::getUInt64Vector(const JsonValue& _value, unsigned int _ix, const std::vector<uint64_t>& _default) {
	std::vector<uint64_t> ret;
	OT_JSON_getListFromArray(_value, _ix, Uint64, ret, return _default);
}

std::vector<uint64_t> ot::json::getUInt64Vector(const ConstJsonArray& _value, unsigned int _ix, const std::vector<uint64_t>& _default) {
	std::vector<uint64_t> ret;
	OT_JSON_getListFromArray(_value, _ix, Uint64, ret, return _default);
}

std::vector<std::string> ot::json::getStringVector(const JsonValue& _value, unsigned int _ix, const std::vector<std::string>& _default) {
	std::vector<std::string> ret;
	OT_JSON_getListFromArray(_value, _ix, String, ret, return _default);
}

std::vector<std::string> ot::json::getStringVector(const ConstJsonArray& _value, unsigned int _ix, const std::vector<std::string>& _default) {
	std::vector<std::string> ret;
	OT_JSON_getListFromArray(_value, _ix, String, ret, return _default);
}

std::vector<ot::ConstJsonObject> ot::json::getObjectVector(const JsonValue& _value, unsigned int _ix) {
	std::vector<ot::ConstJsonObject> ret;
	OT_JSON_getListFromArray(_value, _ix, Object, ret, throw std::exception("Invalid type"));
}

std::vector<ot::ConstJsonObject> ot::json::getObjectVector(const ConstJsonArray& _value, unsigned int _ix) {
	std::vector<ot::ConstJsonObject> ret;
	OT_JSON_getListFromArray(_value, _ix, Object, ret, throw std::exception("Invalid type"));
}

std::vector<ot::ConstJsonArray> ot::json::getArrayVector(const JsonValue& _value, unsigned int _ix) {
	std::vector<ot::ConstJsonArray> ret;
	OT_JSON_getListFromArray(_value, _ix, Array, ret, throw std::exception("Invalid type"));
}

std::vector<ot::ConstJsonArray> ot::json::getArrayVector(const ConstJsonArray& _value, unsigned int _ix) {
	std::vector<ot::ConstJsonArray> ret;
	OT_JSON_getListFromArray(_value, _ix, Array, ret, throw std::exception("Invalid type"));
}
