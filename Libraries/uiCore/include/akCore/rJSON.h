/*
 *	File:		rJSON.h
 *	Package:	akCore
 *
 *  Created on: January 03, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

#include <exception>

// C++ header
#include <string>
#include <list>

#include <rapidjson/document.h>

#define AK_rJSON_doc rapidjson::Document
#define AK_rJSON_val rapidjson::Value
#define AK_rJSON_parseDOC(___doc, ___json) AK_rJSON_doc ___doc; ___doc.Parse(___json);
#define AK_rJSON_createDOC(___doc) AK_rJSON_doc ___doc; ___doc.SetObject();
#define AK_rJSON_createValueObject(___val) AK_rJSON_val ___val; ___val.SetObject();
#define AK_rJSON_createValueArray(___val) AK_rJSON_val ___val(rapidjson::kArrayType);
#define AK_rJSON_docCheck(___doc) if (!___doc.IsObject()) { throw std::exception("Document is not an object"); }
#define AK_rJSON_ifNoMember(___doc, ___member) if (!___doc.HasMember(___member))
#define AK_rJSON_ifMember(___doc, ___member) if (___doc.HasMember(___member))

#define AK_rJSON_toJSON(___doc) ot::rJSON::toJSON(___doc)

namespace ak {
	namespace rJSON {
		
		//! @brief Will check if the provided member is part of the specified doc
		//! @param _doc The document
		//! @param _member The member to check
		//! @throw std::exception if the specified member is missing in the provided document
		__declspec(dllexport) void memberCheck(AK_rJSON_doc & _doc, const char * _member);

		//! @brief Will check if the provided member is part of the specified doc
		//! @param _doc The document
		//! @param _member The member to check
		//! @throw std::exception if the specified member is missing in the provided document
		__declspec(dllexport) void memberCheck(AK_rJSON_val & _doc, const char * _member);

		// ##########################################################################################################
		
		// Document Getter

		//! @brief Will return the string from the specified member in the provided document
		//! @param _doc The document
		//! @param _member The name of the member
		//! @throw std::exception if the member is missing or the member type is not String
		__declspec(dllexport) std::string getString(AK_rJSON_doc & _doc, const char * _member);

		//! @brief Will return the boolean value from the specified member in the provided document
		//! @param _doc The document
		//! @param _member The name of the member
		//! @throw std::exception if the member is missing or the member type is not boolean
		__declspec(dllexport) bool getBool(AK_rJSON_doc & _doc, const char * _member);

		//! @brief Will return the integer value from the specified member in the provided document
		//! @param _doc The document
		//! @param _member The name of the member
		//! @throw std::exception if the member is missing or the member type is not integer
		__declspec(dllexport) int getInt(AK_rJSON_doc & _doc, const char * _member);

		//! @brief Will return the uLongLong from the specified member in the provided document
		//! @param _doc The document
		//! @param _member The name of the member
		//! @throw std::exception if the member is missing or the member type is not uLongLong
		__declspec(dllexport) unsigned long long getULongLong(AK_rJSON_doc & _doc, const char * _member);

		//! @brief Will return the uLongLong list from the specified member in the provided document
		//! @param _doc The document
		//! @param _member The name of the member
		//! @throw std::exception if the member is missing or the member type is not uLongLong
		__declspec(dllexport) std::list<unsigned long long> getULongLongList(AK_rJSON_doc & _doc, const char * _member);

		//! @brief Will return the string list from the specified member in the provided document
		//! @param _doc The document
		//! @param _member The name of the member
		//! @throw std::exception if the member is missing or the member type is not array
		__declspec(dllexport) std::list<std::string> getStringList(AK_rJSON_doc & _doc, const char * _member);

		//! @brief Will return the object list from the specified member in the provided document
		//! @param _doc The document
		//! @param _member The name of the member
		//! @throw std::exception if the member is missing or the member type is not array
		__declspec(dllexport) std::list<std::string> getObjectList(AK_rJSON_doc & _doc, const char * _member);
		
		// ##########################################################################################################
		
		// Document Setter

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		__declspec(dllexport) void add(AK_rJSON_doc & _doc, const std::string & _member, const char * _value);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		__declspec(dllexport) void add(AK_rJSON_doc & _doc, const std::string & _member, const std::string & _value);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		__declspec(dllexport) void add(AK_rJSON_doc & _doc, const std::string & _member, bool _value);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		__declspec(dllexport) void add(AK_rJSON_doc & _doc, const std::string & _member, int _value);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		__declspec(dllexport) void add(AK_rJSON_doc & _doc, const std::string & _member, double _value);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		__declspec(dllexport) void add(AK_rJSON_doc & _doc, const std::string & _member, unsigned long long _value);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		__declspec(dllexport) void add(AK_rJSON_doc & _doc, const std::string & _member, const double *_values, int size);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		__declspec(dllexport) void add(AK_rJSON_doc & _doc, const std::string & _member, const std::list<std::string> &list);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		__declspec(dllexport) void add(AK_rJSON_doc & _doc, const std::string & _member, const std::list<unsigned long long> &list);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		__declspec(dllexport) void add(AK_rJSON_doc & _doc, const std::string & _member, AK_rJSON_val & _value);

		// ##########################################################################################################
		
		// Value Getter

		//! @brief Will return the string from the specified member in the provided value
		//! @param _doc The document
		//! @param _member The name of the member
		//! @throw std::exception if the member is missing or the member type is not String
		__declspec(dllexport) std::string getString(AK_rJSON_val & _doc, const char * _member);

		//! @brief Will return the boolean value from the specified member in the provided value
		//! @param _doc The document
		//! @param _member The name of the member
		//! @throw std::exception if the member is missing or the member type is not boolean
		__declspec(dllexport) bool getBool(AK_rJSON_val & _doc, const char * _member);

		//! @brief Will return the integer value from the specified member in the provided value
		//! @param _doc The document
		//! @param _member The name of the member
		//! @throw std::exception if the member is missing or the member type is not integer
		__declspec(dllexport) int getInt(AK_rJSON_val & _doc, const char * _member);

		//! @brief Will return the uLongLong from the specified member in the provided value
		//! @param _doc The document
		//! @param _member The name of the member
		//! @throw std::exception if the member is missing or the member type is not uLongLong
		__declspec(dllexport) unsigned long long getULongLong(AK_rJSON_val & _doc, const char * _member);

		//! @brief Will return the uLongLong list from the specified member in the provided value
		//! @param _doc The document
		//! @param _member The name of the member
		//! @throw std::exception if the member is missing or the member type is not uLongLong
		__declspec(dllexport) std::list<unsigned long long> getULongLongList(AK_rJSON_val & _doc, const char * _member);

		//! @brief Will return the string list from the specified member in the provided value
		//! @param _doc The document
		//! @param _member The name of the member
		//! @throw std::exception if the member is missing or the member type is not uLongLong
		__declspec(dllexport) std::list<std::string> getStringList(AK_rJSON_val & _doc, const char * _member);

		//! @brief Will return the object list from the specified member in the provided value
		//! @param _doc The document
		//! @param _member The name of the member
		//! @throw std::exception if the member is missing or the member type is not array
		__declspec(dllexport) std::list<std::string> getObjectList(AK_rJSON_val & _doc, const char * _member);

		// ##########################################################################################################
		
		// Value Setter

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		__declspec(dllexport) void add(AK_rJSON_doc & _doc, AK_rJSON_val & _val, const std::string & _member, const char * _value);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		__declspec(dllexport) void add(AK_rJSON_doc & _doc, AK_rJSON_val & _val, const std::string & _member, const std::string & _value);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		__declspec(dllexport) void add(AK_rJSON_doc & _doc, AK_rJSON_val & _val, const std::string & _member, bool _value);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		__declspec(dllexport) void add(AK_rJSON_doc & _doc, AK_rJSON_val & _val, const std::string & _member, int _value);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		__declspec(dllexport) void add(AK_rJSON_doc & _doc, AK_rJSON_val & _val, const std::string & _member, double _value);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		__declspec(dllexport) void add(AK_rJSON_doc & _doc, AK_rJSON_val & _val, const std::string & _member, unsigned long long _value);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		__declspec(dllexport) void add(AK_rJSON_doc & _doc, AK_rJSON_val & _val, const std::string & _member, const double *_values, int size);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		__declspec(dllexport) void add(AK_rJSON_doc & _doc, AK_rJSON_val & _val, const std::string & _member, std::list<std::string> &list);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		__declspec(dllexport) void add(AK_rJSON_doc & _doc, AK_rJSON_val & _val, const std::string & _member, std::list<unsigned long long> &list);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		__declspec(dllexport) void add(AK_rJSON_doc & _doc, AK_rJSON_val & _val, const std::string & _member, AK_rJSON_val &_value);

		// ##########################################################################################################
		
		// Convert

		//! @brief Will return the JSON string created from the provided document
		//! @param _doc The document
		__declspec(dllexport) std::string toJSON(AK_rJSON_doc & _doc);

		//! @brief Will return the JSON string created from the provided document
		//! @param _doc The document
		__declspec(dllexport) std::string toJSON(AK_rJSON_val & _doc);
	}
}
