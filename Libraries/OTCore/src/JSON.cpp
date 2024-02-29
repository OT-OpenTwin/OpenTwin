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


bool ot::json::exists(const JsonValue & _value, const char* _member) {
	return _value.HasMember(_member);
}

bool ot::json::exists(const ConstJsonObject & _value, const char* _member) {
	return _value.HasMember(_member);
}

bool ot::json::isNull(const JsonValue& _value, const char* _member) {
	if (!_value.HasMember(_member)) return false;
	return _value[_member].IsNull();
}

bool ot::json::isNull(const ConstJsonObject& _value, const char* _member) {
	if (!_value.HasMember(_member)) return false;
	return _value[_member].IsNull();
}

bool ot::json::isBool(const JsonValue & _value, const char* _member) {
	if (!_value.HasMember(_member)) return false;
	return _value[_member].IsBool();
}

bool ot::json::isBool(const ConstJsonObject& _value, const char* _member) {
	if (!_value.HasMember(_member)) return false;
	return _value[_member].IsBool();
}

bool ot::json::isInt(const JsonValue& _value, const char* _member) {
	if (!_value.HasMember(_member)) return false;
	return _value[_member].IsInt();
}

bool ot::json::isInt(const ConstJsonObject& _value, const char* _member) {
	if (!_value.HasMember(_member)) return false;
	return _value[_member].IsInt();
}

bool ot::json::isUInt(const JsonValue& _value, const char* _member) {
	if (!_value.HasMember(_member)) return false;
	return _value[_member].IsUint();
}

bool ot::json::isUInt(const ConstJsonObject& _value, const char* _member) {
	if (!_value.HasMember(_member)) return false;
	return _value[_member].IsUint();
}

bool ot::json::isInt64(const JsonValue& _value, const char* _member) {
	if (!_value.HasMember(_member)) return false;
	return _value[_member].IsInt64();
}

bool ot::json::isInt64(const ConstJsonObject& _value, const char* _member) {
	if (!_value.HasMember(_member)) return false;
	return _value[_member].IsInt64();
}

bool ot::json::isUInt64(const JsonValue& _value, const char* _member) {
	if (!_value.HasMember(_member)) return false;
	return _value[_member].IsUint64();
}

bool ot::json::isUInt64(const ConstJsonObject& _value, const char* _member) {
	if (!_value.HasMember(_member)) return false;
	return _value[_member].IsUint64();
}

bool ot::json::isFloat(const JsonValue& _value, const char* _member) {
	if (!_value.HasMember(_member)) return false;
	return _value[_member].IsFloat();
}

bool ot::json::isFloat(const ConstJsonObject& _value, const char* _member) {
	if (!_value.HasMember(_member)) return false;
	return _value[_member].IsFloat();
}

bool ot::json::isDouble(const JsonValue& _value, const char* _member) {
	if (!_value.HasMember(_member)) return false;
	return _value[_member].IsDouble();
}

bool ot::json::isDouble(const ConstJsonObject& _value, const char* _member) {
	if (!_value.HasMember(_member)) return false;
	return _value[_member].IsDouble();
}

bool ot::json::isString(const JsonValue& _value, const char* _member) {
	if (!_value.HasMember(_member)) return false;
	return _value[_member].IsString();
}

bool ot::json::isString(const ConstJsonObject& _value, const char* _member) {
	if (!_value.HasMember(_member)) return false;
	return _value[_member].IsString();
}

bool ot::json::isObject(const JsonValue& _value, const char* _member) {
	if (!_value.HasMember(_member)) return false;
	return _value[_member].IsObject();
}

bool ot::json::isObject(const ConstJsonObject& _value, const char* _member) {
	if (!_value.HasMember(_member)) return false;
	return _value[_member].IsObject();
}

bool ot::json::isArray(const JsonValue& _value, const char* _member) {
	if (!_value.HasMember(_member)) return false;
	return _value[_member].IsArray();
}

bool ot::json::isArray(const ConstJsonObject& _value, const char* _member) {
	if (!_value.HasMember(_member)) return false;
	return _value[_member].IsArray();
}

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

std::list<bool> ot::json::getBoolList(const JsonValue& _value, const char* _member, const std::list<bool>& _default) {
	std::list<bool> ret;
	OT_JSON_getListFromObject(_value, _member, Bool, ret, return _default);
}

std::list<bool> ot::json::getBoolList(const ConstJsonObject& _value, const char* _member, const std::list<bool>& _default) {
	std::list<bool> ret;
	OT_JSON_getListFromObject(_value, _member, Bool, ret, return _default);
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

std::vector<bool> ot::json::getBoolVector(const JsonValue& _value, const char* _member, const std::vector<bool>& _default) {
	std::vector<bool> ret;
	OT_JSON_getListFromObject(_value, _member, Bool, ret, return _default);
}

std::vector<bool> ot::json::getBoolVector(const ConstJsonObject& _value, const char* _member, const std::vector<bool>& _default) {
	std::vector<bool> ret;
	OT_JSON_getListFromObject(_value, _member, Bool, ret, return _default);
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

bool ot::json::isNull(const JsonValue& _value, unsigned int _ix) {
	if (_ix >= _value.Size()) return false;
	return _value[_ix].IsNull();
}

bool ot::json::isNull(const ConstJsonArray& _value, unsigned int _ix) {
	if (_ix >= _value.Size()) return false;
	return _value[_ix].IsNull();
}

bool ot::json::isBool(const JsonValue& _value, unsigned int _ix) {
	if (_ix >= _value.Size()) return false;
	return _value[_ix].IsBool();
}

bool ot::json::isBool(const ConstJsonArray& _value, unsigned int _ix) {
	if (_ix >= _value.Size()) return false;
	return _value[_ix].IsBool();
}

bool ot::json::isInt(const JsonValue& _value, unsigned int _ix) {
	if (_ix >= _value.Size()) return false;
	return _value[_ix].IsInt();
}

bool ot::json::isInt(const ConstJsonArray& _value, unsigned int _ix) {
	if (_ix >= _value.Size()) return false;
	return _value[_ix].IsInt();
}

bool ot::json::isUInt(const JsonValue& _value, unsigned int _ix) {
	if (_ix >= _value.Size()) return false;
	return _value[_ix].IsUint();
}

bool ot::json::isUInt(const ConstJsonArray& _value, unsigned int _ix) {
	if (_ix >= _value.Size()) return false;
	return _value[_ix].IsUint();
}

bool ot::json::isInt64(const JsonValue& _value, unsigned int _ix) {
	if (_ix >= _value.Size()) return false;
	return _value[_ix].IsInt64();
}

bool ot::json::isInt64(const ConstJsonArray& _value, unsigned int _ix) {
	if (_ix >= _value.Size()) return false;
	return _value[_ix].IsInt64();
}

bool ot::json::isUInt64(const JsonValue& _value, unsigned int _ix) {
	if (_ix >= _value.Size()) return false;
	return _value[_ix].IsUint64();
}

bool ot::json::isUInt64(const ConstJsonArray& _value, unsigned int _ix) {
	if (_ix >= _value.Size()) return false;
	return _value[_ix].IsUint64();
}

bool ot::json::isFloat(const JsonValue& _value, unsigned int _ix) {
	if (_ix >= _value.Size()) return false;
	return _value[_ix].IsFloat();
}

bool ot::json::isFloat(const ConstJsonArray& _value, unsigned int _ix) {
	if (_ix >= _value.Size()) return false;
	return _value[_ix].IsFloat();
}

bool ot::json::isDouble(const JsonValue& _value, unsigned int _ix) {
	if (_ix >= _value.Size()) return false;
	return _value[_ix].IsDouble();
}

bool ot::json::isDouble(const ConstJsonArray& _value, unsigned int _ix) {
	if (_ix >= _value.Size()) return false;
	return _value[_ix].IsDouble();
}

bool ot::json::isString(const JsonValue& _value, unsigned int _ix) {
	if (_ix >= _value.Size()) return false;
	return _value[_ix].IsString();
}

bool ot::json::isString(const ConstJsonArray& _value, unsigned int _ix) {
	if (_ix >= _value.Size()) return false;
	return _value[_ix].IsString();
}

bool ot::json::isObject(const JsonValue& _value, unsigned int _ix) {
	if (_ix >= _value.Size()) return false;
	return _value[_ix].IsObject();
}

bool ot::json::isObject(const ConstJsonArray& _value, unsigned int _ix) {
	if (_ix >= _value.Size()) return false;
	return _value[_ix].IsObject();
}

bool ot::json::isArray(const JsonValue& _value, unsigned int _ix) {
	if (_ix >= _value.Size()) return false;
	return _value[_ix].IsArray();
}

bool ot::json::isArray(const ConstJsonArray& _value, unsigned int _ix) {
	if (_ix >= _value.Size()) return false;
	return _value[_ix].IsArray();
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
