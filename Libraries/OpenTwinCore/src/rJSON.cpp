/*
 *	rJSON.cpp
 *
 *  Created on: January 03, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2023 OpenTwin
 */

#include "OpenTwinCore/rJSON.h"				// Corresponding header

#include <rapidjson/stringbuffer.h>		// StringBuffer
#include <rapidjson/writer.h>			// Writer

void ot::rJSON::memberCheck(OT_rJSON_doc & _doc, const char * _member) {
	if (!_doc.HasMember(_member)) {
		std::string error("Member \"");
		error.append(_member);
		error.append("\" is missing");
		throw std::exception(error.c_str());
	}
}

void ot::rJSON::memberCheck(OT_rJSON_val & _doc, const char * _member) {
	if (!_doc.HasMember(_member)) {
		std::string error("Member \"");
		error.append(_member);
		error.append("\" is missing");
		throw std::exception(error.c_str());
	}
}

OT_CORE_API_EXPORT bool ot::rJSON::memberExists(OT_rJSON_val& _doc, const char* _member)
{
	return _doc.HasMember(_member);
}

// ##########################################################################################################

// Document Getter

std::string ot::rJSON::getString(OT_rJSON_doc & _doc, const char * _member) {
	memberCheck(_doc, _member);
	if (!_doc[_member].IsString()) {
		std::string error("Member \"");
		error.append(_member);
		error.append("\" is not a string type");
		throw std::exception(error.c_str());
	}
	return _doc[_member].GetString();
}

bool ot::rJSON::getBool(OT_rJSON_doc & _doc, const char * _member) {
	memberCheck(_doc, _member);
	if (!_doc[_member].IsBool()) {
		std::string error("Member \"");
		error.append(_member);
		error.append("\" is not a boolean type");
		throw std::exception(error.c_str());
	}
	return _doc[_member].GetBool();
}

int ot::rJSON::getInt(OT_rJSON_doc & _doc, const char * _member) {
	memberCheck(_doc, _member);
	if (!_doc[_member].IsInt()) {
		std::string error("Member \"");
		error.append(_member);
		error.append("\" is not a integer type");
		throw std::exception(error.c_str());
	}
	return _doc[_member].GetInt();
}

unsigned int ot::rJSON::getUInt(OT_rJSON_doc & _doc, const char * _member) {
	memberCheck(_doc, _member);
	if (!_doc[_member].IsUint()) {
		std::string error("Member \"");
		error.append(_member);
		error.append("\" is not a unsigned integer type");
		throw std::exception(error.c_str());
	}
	return _doc[_member].GetUint();
}

unsigned long long ot::rJSON::getULongLong(OT_rJSON_doc & _doc, const char * _member) {
	memberCheck(_doc, _member);
	if (!_doc[_member].IsUint64()) {
		std::string error("Member \"");
		error.append(_member);
		error.append("\" is not a unsigned long long type");
		throw std::exception(error.c_str());
	}
	return _doc[_member].GetUint64();
}

float ot::rJSON::getFloat(OT_rJSON_doc & _doc, const char * _member) {
	memberCheck(_doc, _member);
	if (!_doc[_member].IsFloat()) {
		std::string error("Member \"");
		error.append(_member);
		error.append("\" is not a float type");
		throw std::exception(error.c_str());
	}
	return _doc[_member].GetFloat();
}

double ot::rJSON::getDouble(OT_rJSON_doc & _doc, const char * _member) {
	memberCheck(_doc, _member);
	if (!_doc[_member].IsDouble()) {
		std::string error("Member \"");
		error.append(_member);
		error.append("\" is not a double type");
		throw std::exception(error.c_str());
	}
	return _doc[_member].GetDouble();
}

OT_rJSON_val ot::rJSON::getObject(OT_rJSON_doc& _doc, const char* _member)
{
	memberCheck(_doc, _member);
	if (!_doc[_member].IsObject())
	{
		std::string error("Member \"");
		error.append(_member);
		error.append("\" is not an Object type");
		throw std::exception(error.c_str());
	}
	return _doc[_member].GetObject();
}

std::list<unsigned long long> ot::rJSON::getULongLongList(OT_rJSON_doc & _doc, const char * _member) {
	memberCheck(_doc, _member);
	if (!_doc[_member].IsArray()) {
		std::string error("Member \"");
		error.append(_member);
		error.append("\" is not a unsigned long long list type");
		throw std::exception(error.c_str());
	}

	std::list<unsigned long long> result;

	rapidjson::Value list = _doc[_member].GetArray();

	for (unsigned int i = 0; i < list.Size(); i++)
	{
		result.push_back(list[i].GetUint64());
	}

	return result;
}

std::list<std::string> ot::rJSON::getStringList(OT_rJSON_doc & _doc, const char * _member) {
	memberCheck(_doc, _member);
	if (!_doc[_member].IsArray()) {
		std::string error("Member \"");
		error.append(_member);
		error.append("\" is not a string list type");
		throw std::exception(error.c_str());
	}

	std::list<std::string> result;

	rapidjson::Value list = _doc[_member].GetArray();

	for (unsigned int i = 0; i < list.Size(); i++)
	{
		result.push_back(list[i].GetString());
	}

	return result;
}

std::list<std::string> ot::rJSON::getObjectList(OT_rJSON_doc & _doc, const char * _member) {
	memberCheck(_doc, _member);
	assert(_doc[_member].IsArray());
	auto services = _doc[_member].GetArray();
	std::list<std::string> result;
	for (unsigned int i = 0; i < services.Size(); i++) {
		OT_rJSON_val v = services[i].GetObject();
		result.push_back(ot::rJSON::toJSON(v));
	}
	return result;
}

std::vector<double> ot::rJSON::getDoubleVector(OT_rJSON_doc & _doc, const char * _member) {

	memberCheck(_doc, _member);
	if (!_doc[_member].IsArray()) {
		std::string error("Member \"");
		error.append(_member);
		error.append("\" is not a string list type");
		throw std::exception(error.c_str());
	}

	rapidjson::Value list = _doc[_member].GetArray();

	std::vector<double> result;
	result.reserve(list.Size());

	for (unsigned int i = 0; i < list.Size(); i++)
	{
		result.push_back(list[i].GetDouble());
	}

	return result;
}

std::vector<int> ot::rJSON::getIntVector(OT_rJSON_doc & _doc, const char * _member) {

	memberCheck(_doc, _member);
	if (!_doc[_member].IsArray()) {
		std::string error("Member \"");
		error.append(_member);
		error.append("\" is not a string list type");
		throw std::exception(error.c_str());
	}

	rapidjson::Value list = _doc[_member].GetArray();

	std::vector<int> result;
	result.reserve(list.Size());

	for (unsigned int i = 0; i < list.Size(); i++)
	{
		result.push_back(list[i].GetInt());
	}

	return result;
}

// ##########################################################################################################

// Document Setter

void ot::rJSON::add(OT_rJSON_doc & _doc, const std::string & _member, const std::string & _value) {
	_doc.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), rapidjson::Value(_value.c_str(), _doc.GetAllocator()), _doc.GetAllocator());
}

void ot::rJSON::add(OT_rJSON_doc & _doc, const std::string & _member, const char * _value) {
	_doc.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), rapidjson::Value(_value, _doc.GetAllocator()), _doc.GetAllocator());
}

void ot::rJSON::add(OT_rJSON_doc & _doc, const std::string & _member, bool _value) {
	_doc.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), rapidjson::Value(_value), _doc.GetAllocator());
}

void ot::rJSON::add(OT_rJSON_doc & _doc, const std::string & _member, int _value) {
	_doc.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), rapidjson::Value(_value), _doc.GetAllocator());
}

void ot::rJSON::add(OT_rJSON_doc & _doc, const std::string & _member, unsigned int _value) {
	_doc.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), rapidjson::Value(_value), _doc.GetAllocator());
}

void ot::rJSON::add(OT_rJSON_doc & _doc, const std::string & _member, float _value) {
	_doc.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), rapidjson::Value(_value), _doc.GetAllocator());
}

void ot::rJSON::add(OT_rJSON_doc & _doc, const std::string & _member, double _value) {
	_doc.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), rapidjson::Value(_value), _doc.GetAllocator());
}

void ot::rJSON::add(OT_rJSON_doc & _doc, const std::string & _member, unsigned long long _value) {
	_doc.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), rapidjson::Value(_value), _doc.GetAllocator());
}

OT_CORE_API_EXPORTONLY void ot::rJSON::add(OT_rJSON_doc& _doc, const std::string& _member, long long _value){
	_doc.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), rapidjson::Value(_value), _doc.GetAllocator());
}

void ot::rJSON::add(OT_rJSON_doc & _doc, const std::string & _member, const double *_values, int size) {
	rapidjson::Value doubleArr(rapidjson::kArrayType);
	doubleArr.Reserve(size, _doc.GetAllocator());
	for (int i = 0; i < size; i++)
	{
		doubleArr.PushBack(rapidjson::Value().SetDouble(_values[i]), _doc.GetAllocator());
	}

	_doc.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), doubleArr, _doc.GetAllocator());
}

void ot::rJSON::add(OT_rJSON_doc & _doc, const std::string & _member, const std::list<std::string> &list)
{
	rapidjson::Value listUID(rapidjson::kArrayType);
	for (auto id : list)
	{
		listUID.PushBack(rapidjson::Value(id.c_str(), _doc.GetAllocator()), _doc.GetAllocator());
	}

	_doc.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), listUID, _doc.GetAllocator());
}

void ot::rJSON::add(OT_rJSON_doc & _doc, const std::string & _member, const std::list<unsigned long long> &list)
{
	rapidjson::Value listUID(rapidjson::kArrayType);
	for (auto id : list)
	{
		listUID.PushBack(rapidjson::Value(id), _doc.GetAllocator());
	}

	_doc.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), listUID, _doc.GetAllocator());
}

void ot::rJSON::add(OT_rJSON_doc & _doc, const std::string & _member, const std::list<bool> &list)
{
	rapidjson::Value listUID(rapidjson::kArrayType);
	for (auto id : list)
	{
		listUID.PushBack(rapidjson::Value(id), _doc.GetAllocator());
	}

	_doc.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), listUID, _doc.GetAllocator());
}

void ot::rJSON::add(OT_rJSON_doc & _doc, const std::string & _member, OT_rJSON_val & _value) {
	_doc.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), _value, _doc.GetAllocator());
}

void ot::rJSON::add(OT_rJSON_doc & _doc, const std::string & _member, const std::vector<double> &array)
{
	rapidjson::Value listValue(rapidjson::kArrayType);
	for (auto value : array)
	{
		listValue.PushBack(rapidjson::Value(value), _doc.GetAllocator());
	}

	_doc.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), listValue, _doc.GetAllocator());
}

void ot::rJSON::add(OT_rJSON_doc & _doc, const std::string & _member, const std::vector<int> &array)
{
	rapidjson::Value listValue(rapidjson::kArrayType);
	for (auto value : array)
	{
		listValue.PushBack(rapidjson::Value(value), _doc.GetAllocator());
	}

	_doc.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), listValue, _doc.GetAllocator());
}

// ##########################################################################################################

// Value Getter

std::string ot::rJSON::getString(OT_rJSON_val & _doc, const char * _member) {
	memberCheck(_doc, _member);
	if (!_doc[_member].IsString()) {
		std::string error("Member \"");
		error.append(_member);
		error.append("\" is not a string type");
		throw std::exception(error.c_str());
	}
	return _doc[_member].GetString();
}

bool ot::rJSON::getBool(OT_rJSON_val & _doc, const char * _member) {
	memberCheck(_doc, _member);
	if (!_doc[_member].IsBool()) {
		std::string error("Member \"");
		error.append(_member);
		error.append("\" is not a boolean type");
		throw std::exception(error.c_str());
	}
	return _doc[_member].GetBool();
}

int ot::rJSON::getInt(OT_rJSON_val & _doc, const char * _member) {
	memberCheck(_doc, _member);
	if (!_doc[_member].IsInt()) {
		std::string error("Member \"");
		error.append(_member);
		error.append("\" is not a integer type");
		throw std::exception(error.c_str());
	}
	return _doc[_member].GetInt();
}

unsigned int ot::rJSON::getUInt(OT_rJSON_val & _doc, const char * _member) {
	memberCheck(_doc, _member);
	if (!_doc[_member].IsUint()) {
		std::string error("Member \"");
		error.append(_member);
		error.append("\" is not a unsigned integer type");
		throw std::exception(error.c_str());
	}
	return _doc[_member].GetUint();
}

unsigned long long ot::rJSON::getULongLong(OT_rJSON_val & _doc, const char * _member) {
	memberCheck(_doc, _member);
	if (!_doc[_member].IsUint64()) {
		std::string error("Member \"");
		error.append(_member);
		error.append("\" is not a unsigned long long type");
		throw std::exception(error.c_str());
	}
	return _doc[_member].GetUint64();
}

float ot::rJSON::getFloat(OT_rJSON_val & _doc, const char * _member) {
	memberCheck(_doc, _member);
	if (!_doc[_member].IsFloat()) {
		std::string error("Member \"");
		error.append(_member);
		error.append("\" is not a float type");
		throw std::exception(error.c_str());
	}
	return _doc[_member].GetFloat();
}

double ot::rJSON::getDouble(OT_rJSON_val & _doc, const char * _member) {
	memberCheck(_doc, _member);
	if (!_doc[_member].IsDouble()) {
		std::string error("Member \"");
		error.append(_member);
		error.append("\" is not a double type");
		throw std::exception(error.c_str());
	}
	return _doc[_member].GetDouble();
}

std::list<unsigned long long> ot::rJSON::getULongLongList(OT_rJSON_val & _doc, const char * _member) {
	memberCheck(_doc, _member);
	if (!_doc[_member].IsArray()) {
		std::string error("Member \"");
		error.append(_member);
		error.append("\" is not a unsigned long long list type");
		throw std::exception(error.c_str());
	}

	std::list<unsigned long long> result;

	rapidjson::Value list = _doc[_member].GetArray();

	for (unsigned int i = 0; i < list.Size(); i++)
	{
		result.push_back(list[i].GetUint64());
	}

	return result;
}

std::list<std::string> ot::rJSON::getStringList(OT_rJSON_val & _doc, const char * _member) {
	memberCheck(_doc, _member);
	if (!_doc[_member].IsArray()) {
		std::string error("Member \"");
		error.append(_member);
		error.append("\" is not a string list type");
		throw std::exception(error.c_str());
	}

	std::list<std::string> result;

	rapidjson::Value list = _doc[_member].GetArray();

	for (unsigned int i = 0; i < list.Size(); i++)
	{
		result.push_back(list[i].GetString());
	}

	return result;
}

std::list<std::string> ot::rJSON::getObjectList(OT_rJSON_val & _doc, const char * _member) {
	memberCheck(_doc, _member);
	assert(_doc[_member].IsArray());
	auto services = _doc[_member].GetArray();
	std::list<std::string> result;
	for (unsigned int i = 0; i < services.Size(); i++) {
		OT_rJSON_val v = services[i].GetObject();
		result.push_back(ot::rJSON::toJSON(v));
	}
	return result;
}

OT_CORE_API_EXPORTONLY bool ot::rJSON::isBool(OT_rJSON_val& _doc, const char* _member)
{
	return _doc[_member].IsBool();
}

OT_CORE_API_EXPORTONLY bool ot::rJSON::isString(OT_rJSON_val& _doc, const char* _member)
{
	return _doc[_member].IsString();
}

OT_CORE_API_EXPORTONLY bool ot::rJSON::isInteger(OT_rJSON_val& _doc, const char* _member)
{
	return _doc[_member].IsInt();
}

OT_CORE_API_EXPORTONLY bool ot::rJSON::isUInt(OT_rJSON_val& _doc, const char* _member)
{
	return _doc[_member].IsUint();
}

OT_CORE_API_EXPORTONLY bool ot::rJSON::isULongLong(OT_rJSON_val& _doc, const char* _member)
{
	return _doc[_member].IsUint64();
}

OT_CORE_API_EXPORTONLY bool ot::rJSON::isFloat(OT_rJSON_val& _doc, const char* _member)
{
	return _doc[_member].IsFloat();
}

OT_CORE_API_EXPORTONLY bool ot::rJSON::isDouble(OT_rJSON_val& _doc, const char* _member)
{
	return _doc[_member].IsDouble();
}

OT_CORE_API_EXPORTONLY bool ot::rJSON::isArray(OT_rJSON_val& _doc, const char* _member)
{
	return _doc[_member].IsArray();
}
OT_CORE_API_EXPORTONLY bool ot::rJSON::isStringList(OT_rJSON_val& _doc, const char* _member)
{
	const bool typeCorrect = isArray(_doc, _member);
	if (typeCorrect)
	{
		auto array = _doc[_member].GetArray();
		if (array.Size() != 0)
		{
			return array[0].IsString();
		}
		else
		{
			return true;
		}
	}
	else
	{
		return false;
	}
}
// ##########################################################################################################

// Value Setter

void ot::rJSON::add(OT_rJSON_doc & _doc, OT_rJSON_val & _val, const std::string & _member, const std::string & _value) {
	_val.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), rapidjson::Value(_value.c_str(), _doc.GetAllocator()), _doc.GetAllocator());
}

void ot::rJSON::add(OT_rJSON_doc & _doc, OT_rJSON_val & _val, const std::string & _member, const char * _value) {
	_val.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), rapidjson::Value(_value, _doc.GetAllocator()), _doc.GetAllocator());
}

void ot::rJSON::add(OT_rJSON_doc & _doc, OT_rJSON_val & _val, const std::string & _member, bool _value) {
	_val.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), rapidjson::Value(_value), _doc.GetAllocator());
}

void ot::rJSON::add(OT_rJSON_doc & _doc, OT_rJSON_val & _val, const std::string & _member, int _value) {
	_val.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), rapidjson::Value(_value), _doc.GetAllocator());
}

void ot::rJSON::add(OT_rJSON_doc & _doc, OT_rJSON_val & _val, const std::string & _member, unsigned int _value) {
	_val.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), rapidjson::Value(_value), _doc.GetAllocator());
}

void ot::rJSON::add(OT_rJSON_doc & _doc, OT_rJSON_val & _val, const std::string & _member, float _value) {
	_val.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), rapidjson::Value(_value), _doc.GetAllocator());
}

void ot::rJSON::add(OT_rJSON_doc & _doc, OT_rJSON_val & _val, const std::string & _member, double _value) {
	_val.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), rapidjson::Value(_value), _doc.GetAllocator());
}

void ot::rJSON::add(OT_rJSON_doc & _doc, OT_rJSON_val & _val, const std::string & _member, unsigned long long _value) {
	_val.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), rapidjson::Value(_value), _doc.GetAllocator());
}

void ot::rJSON::add(OT_rJSON_doc & _doc, OT_rJSON_val & _val, const std::string & _member, OT_rJSON_val &_value) {
	_val.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), _value, _doc.GetAllocator());
}

void ot::rJSON::add(OT_rJSON_doc & _doc, OT_rJSON_val & _val, const std::string & _member, const double *_values, int size) {
	rapidjson::Value doubleArr(rapidjson::kArrayType);
	doubleArr.Reserve(size, _doc.GetAllocator());
	for (int i = 0; i < size; i++)
	{
		doubleArr.PushBack(rapidjson::Value().SetDouble(_values[i]), _doc.GetAllocator());
	}

	_val.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), doubleArr, _doc.GetAllocator());
}

void ot::rJSON::add(OT_rJSON_doc & _doc, OT_rJSON_val & _val, const std::string & _member, const std::list<std::string> &list)
{
	rapidjson::Value listUID(rapidjson::kArrayType);
	for (auto id : list)
	{
		listUID.PushBack(rapidjson::Value(id.c_str(), _doc.GetAllocator()), _doc.GetAllocator());
	}

	_val.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), listUID, _doc.GetAllocator());
}

void ot::rJSON::add(OT_rJSON_doc & _doc, OT_rJSON_val & _val, const std::string & _member, const std::list<unsigned long long> &list)
{
	rapidjson::Value listUID(rapidjson::kArrayType);
	for (auto id : list)
	{
		listUID.PushBack(rapidjson::Value(id), _doc.GetAllocator());
	}

	_val.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), listUID, _doc.GetAllocator());
}

// ****************************************************************************************************

// Convert

std::string ot::rJSON::toJSON(OT_rJSON_doc & _doc) {
	// Create String buffer
	rapidjson::StringBuffer buffer;
	buffer.Clear();

	// Setup the Writer
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	_doc.Accept(writer);

	// Return string
	return buffer.GetString();
}

std::string ot::rJSON::toJSON(OT_rJSON_val & _doc) {
	// Create String buffer
	rapidjson::StringBuffer buffer;
	buffer.Clear();

	// Setup the Writer
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	_doc.Accept(writer);

	// Return string
	return buffer.GetString();
}

OT_CORE_API_EXPORT OT_rJSON_doc ot::rJSON::fromJSON(const std::string& jsonString)
{
	OT_rJSON_createDOC(doc);
	doc.Parse(jsonString.c_str());
	return doc;
}
