/*
 *	File:		rJSON.cpp
 *	Package:	akCore
 *
 *  Created on: January 03, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#include <akCore/rJSON.h>

#include <rapidjson/stringbuffer.h>		// StringBuffer
#include <rapidjson/writer.h>			// Writer

void ak::rJSON::memberCheck(AK_rJSON_doc & _doc, const char * _member) {
	if (!_doc.HasMember(_member)) {
		std::string error("Member \"");
		error.append(_member);
		error.append("\" is missing");
		throw std::exception(error.c_str());
	}
}

void ak::rJSON::memberCheck(AK_rJSON_val & _doc, const char * _member) {
	if (!_doc.HasMember(_member)) {
		std::string error("Member \"");
		error.append(_member);
		error.append("\" is missing");
		throw std::exception(error.c_str());
	}
}

// ##########################################################################################################

// Document Getter

std::string ak::rJSON::getString(AK_rJSON_doc & _doc, const char * _member) {
	memberCheck(_doc, _member);
	if (!_doc[_member].IsString()) {
		std::string error("Member \"");
		error.append(_member);
		error.append("\" is not a string type");
		throw std::exception(error.c_str());
	}
	return _doc[_member].GetString();
}

bool ak::rJSON::getBool(AK_rJSON_doc & _doc, const char * _member) {
	memberCheck(_doc, _member);
	if (!_doc[_member].IsBool()) {
		std::string error("Member \"");
		error.append(_member);
		error.append("\" is not a boolean type");
		throw std::exception(error.c_str());
	}
	return _doc[_member].GetBool();
}

int ak::rJSON::getInt(AK_rJSON_doc & _doc, const char * _member) {
	memberCheck(_doc, _member);
	if (!_doc[_member].IsInt()) {
		std::string error("Member \"");
		error.append(_member);
		error.append("\" is not a integer type");
		throw std::exception(error.c_str());
	}
	return _doc[_member].GetInt();
}

unsigned long long ak::rJSON::getULongLong(AK_rJSON_doc & _doc, const char * _member) {
	memberCheck(_doc, _member);
	if (!_doc[_member].IsUint64()) {
		std::string error("Member \"");
		error.append(_member);
		error.append("\" is not a unsigned long long type");
		throw std::exception(error.c_str());
	}
	return _doc[_member].GetUint64();
}

std::list<unsigned long long> ak::rJSON::getULongLongList(AK_rJSON_doc & _doc, const char * _member) {
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

std::list<std::string> ak::rJSON::getStringList(AK_rJSON_doc & _doc, const char * _member) {
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

std::list<std::string> ak::rJSON::getObjectList(AK_rJSON_doc & _doc, const char * _member) {
	memberCheck(_doc, _member);
	assert(_doc[_member].IsArray());
	auto services = _doc[_member].GetArray();
	std::list<std::string> result;
	for (unsigned int i = 0; i < services.Size(); i++) {
		AK_rJSON_val v = services[i].GetObject();
		result.push_back(ak::rJSON::toJSON(v));
	}
	return result;
}

// ##########################################################################################################

// Document Setter

void ak::rJSON::add(AK_rJSON_doc & _doc, const std::string & _member, const std::string & _value) {
	_doc.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), rapidjson::Value(_value.c_str(), _doc.GetAllocator()), _doc.GetAllocator());
}

void ak::rJSON::add(AK_rJSON_doc & _doc, const std::string & _member, const char * _value) {
	_doc.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), rapidjson::Value(_value, _doc.GetAllocator()), _doc.GetAllocator());
}

void ak::rJSON::add(AK_rJSON_doc & _doc, const std::string & _member, bool _value) {
	_doc.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), rapidjson::Value(_value), _doc.GetAllocator());
}

void ak::rJSON::add(AK_rJSON_doc & _doc, const std::string & _member, int _value) {
	_doc.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), rapidjson::Value(_value), _doc.GetAllocator());
}

void ak::rJSON::add(AK_rJSON_doc & _doc, const std::string & _member, double _value) {
	_doc.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), rapidjson::Value(_value), _doc.GetAllocator());
}

void ak::rJSON::add(AK_rJSON_doc & _doc, const std::string & _member, unsigned long long _value) {
	_doc.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), rapidjson::Value(_value), _doc.GetAllocator());
}

void ak::rJSON::add(AK_rJSON_doc & _doc, const std::string & _member, const double *_values, int size) {
	rapidjson::Value doubleArr(rapidjson::kArrayType);
	doubleArr.Reserve(size, _doc.GetAllocator());
	for (int i = 0; i < size; i++)
	{
		doubleArr.PushBack(rapidjson::Value().SetDouble(_values[i]), _doc.GetAllocator());
	}

	_doc.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), doubleArr, _doc.GetAllocator());
}

void ak::rJSON::add(AK_rJSON_doc & _doc, const std::string & _member, const std::list<std::string> &list)
{
	rapidjson::Value listUID(rapidjson::kArrayType);
	for (auto id : list)
	{
		listUID.PushBack(rapidjson::Value(id.c_str(), _doc.GetAllocator()), _doc.GetAllocator());
	}

	_doc.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), listUID, _doc.GetAllocator());
}

void ak::rJSON::add(AK_rJSON_doc & _doc, const std::string & _member, const std::list<unsigned long long> &list)
{
	rapidjson::Value listUID(rapidjson::kArrayType);
	for (auto id : list)
	{
		listUID.PushBack(rapidjson::Value(id), _doc.GetAllocator());
	}

	_doc.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), listUID, _doc.GetAllocator());
}

void ak::rJSON::add(AK_rJSON_doc & _doc, const std::string & _member, AK_rJSON_val & _value) {
	_doc.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), _value, _doc.GetAllocator());
}

// ##########################################################################################################

// Value Getter

std::string ak::rJSON::getString(AK_rJSON_val & _doc, const char * _member) {
	memberCheck(_doc, _member);
	if (!_doc[_member].IsString()) {
		std::string error("Member \"");
		error.append(_member);
		error.append("\" is not a string type");
		throw std::exception(error.c_str());
	}
	return _doc[_member].GetString();
}

bool ak::rJSON::getBool(AK_rJSON_val & _doc, const char * _member) {
	memberCheck(_doc, _member);
	if (!_doc[_member].IsBool()) {
		std::string error("Member \"");
		error.append(_member);
		error.append("\" is not a boolean type");
		throw std::exception(error.c_str());
	}
	return _doc[_member].GetBool();
}

int ak::rJSON::getInt(AK_rJSON_val & _doc, const char * _member) {
	memberCheck(_doc, _member);
	if (!_doc[_member].IsInt()) {
		std::string error("Member \"");
		error.append(_member);
		error.append("\" is not a integer type");
		throw std::exception(error.c_str());
	}
	return _doc[_member].GetInt();
}

unsigned long long ak::rJSON::getULongLong(AK_rJSON_val & _doc, const char * _member) {
	memberCheck(_doc, _member);
	if (!_doc[_member].IsUint64()) {
		std::string error("Member \"");
		error.append(_member);
		error.append("\" is not a unsigned long long type");
		throw std::exception(error.c_str());
	}
	return _doc[_member].GetUint64();
}

std::list<unsigned long long> ak::rJSON::getULongLongList(AK_rJSON_val & _doc, const char * _member) {
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

std::list<std::string> ak::rJSON::getStringList(AK_rJSON_val & _doc, const char * _member) {
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

std::list<std::string> ak::rJSON::getObjectList(AK_rJSON_val & _doc, const char * _member) {
	memberCheck(_doc, _member);
	assert(_doc[_member].IsArray());
	auto services = _doc[_member].GetArray();
	std::list<std::string> result;
	for (unsigned int i = 0; i < services.Size(); i++) {
		AK_rJSON_val v = services[i].GetObject();
		result.push_back(ak::rJSON::toJSON(v));
	}
	return result;
}

// ##########################################################################################################

// Value Setter

void ak::rJSON::add(AK_rJSON_doc & _doc, AK_rJSON_val & _val, const std::string & _member, const std::string & _value) {
	_val.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), rapidjson::Value(_value.c_str(), _doc.GetAllocator()), _doc.GetAllocator());
}

void ak::rJSON::add(AK_rJSON_doc & _doc, AK_rJSON_val & _val, const std::string & _member, const char * _value) {
	_val.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), rapidjson::Value(_value, _doc.GetAllocator()), _doc.GetAllocator());
}

void ak::rJSON::add(AK_rJSON_doc & _doc, AK_rJSON_val & _val, const std::string & _member, bool _value) {
	_val.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), rapidjson::Value(_value), _doc.GetAllocator());
}

void ak::rJSON::add(AK_rJSON_doc & _doc, AK_rJSON_val & _val, const std::string & _member, int _value) {
	_val.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), rapidjson::Value(_value), _doc.GetAllocator());
}

void ak::rJSON::add(AK_rJSON_doc & _doc, AK_rJSON_val & _val, const std::string & _member, double _value) {
	_val.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), rapidjson::Value(_value), _doc.GetAllocator());
}

void ak::rJSON::add(AK_rJSON_doc & _doc, AK_rJSON_val & _val, const std::string & _member, unsigned long long _value) {
	_val.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), rapidjson::Value(_value), _doc.GetAllocator());
}

void ak::rJSON::add(AK_rJSON_doc & _doc, AK_rJSON_val & _val, const std::string & _member, AK_rJSON_val &_value) {
	_val.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), _value, _doc.GetAllocator());
}

void ak::rJSON::add(AK_rJSON_doc & _doc, AK_rJSON_val & _val, const std::string & _member, const double *_values, int size) {
	rapidjson::Value doubleArr(rapidjson::kArrayType);
	doubleArr.Reserve(size, _doc.GetAllocator());
	for (int i = 0; i < size; i++)
	{
		doubleArr.PushBack(rapidjson::Value().SetDouble(_values[i]), _doc.GetAllocator());
	}

	_val.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), doubleArr, _doc.GetAllocator());
}

void ak::rJSON::add(AK_rJSON_doc & _doc, AK_rJSON_val & _val, const std::string & _member, std::list<std::string> &list)
{
	rapidjson::Value listUID(rapidjson::kArrayType);
	for (auto id : list)
	{
		listUID.PushBack(rapidjson::Value(id.c_str(), _doc.GetAllocator()), _doc.GetAllocator());
	}

	_val.AddMember(rapidjson::Value(_member.c_str(), _doc.GetAllocator()), listUID, _doc.GetAllocator());
}

void ak::rJSON::add(AK_rJSON_doc & _doc, AK_rJSON_val & _val, const std::string & _member, std::list<unsigned long long> &list)
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

std::string ak::rJSON::toJSON(AK_rJSON_doc & _doc) {
	// Create String buffer
	rapidjson::StringBuffer buffer;
	buffer.Clear();

	// Setup the Writer
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	_doc.Accept(writer);

	// Return string
	return buffer.GetString();
}

std::string ak::rJSON::toJSON(AK_rJSON_val & _doc) {
	// Create String buffer
	rapidjson::StringBuffer buffer;
	buffer.Clear();

	// Setup the Writer
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	_doc.Accept(writer);

	// Return string
	return buffer.GetString();
}