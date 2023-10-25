/*
 *	rJSON.h
 *
 *  Created on: January 03, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2020 OpenTwin
 */

#pragma once

// OpenTwin header
#include "OpenTwinCore/CoreAPIExport.h"

// std header
#include <exception>
#include <string>
#include <list>
#include <vector>

// rapidjson header
#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>

//! @brief JSON document
#define OT_rJSON_doc rapidjson::Document

//! @brief JSON value
#define OT_rJSON_val rapidjson::Value

//! @
#define OT_rJSON_parseDOC(___doc, ___json) OT_rJSON_doc ___doc; ___doc.Parse(___json);
#define OT_rJSON_createDOC(___doc) OT_rJSON_doc ___doc; ___doc.SetObject();
#define OT_rJSON_createValueObject(___val) OT_rJSON_val ___val; ___val.SetObject();
#define OT_rJSON_createValueNull(___val) OT_rJSON_val ___val; ___val.SetNull();
#define OT_rJSON_createValueArray(___val) OT_rJSON_val ___val(rapidjson::kArrayType);
#define OT_rJSON_docCheck(___doc) if (!___doc.IsObject()) { throw std::exception("Document is not an object"); }
#define OT_rJSON_ifNoMember(___doc, ___member) if (!___doc.HasMember(___member))
#define OT_rJSON_ifMember(___doc, ___member) if (___doc.HasMember(___member))

#define OT_rJSON_toJSON(___doc) ot::rJSON::toJSON(___doc)



namespace ot {
	namespace rJSON {
		
		//typedef rapidjson::Document Document;
		//typedef rapidjson::Value Value;

		//! @brief Will check if the provided member is part of the specified doc
		//! @param _doc The document
		//! @param _member The member to check
		//! @throw std::exception if the specified member is missing in the provided document
		OT_CORE_API_EXPORTONLY void memberCheck(OT_rJSON_doc & _doc, const char * _member);

		//! @brief Will check if the provided member is part of the specified doc
		//! @param _doc The document
		//! @param _member The member to check
		//! @throw std::exception if the specified member is missing in the provided document
		OT_CORE_API_EXPORTONLY void memberCheck(OT_rJSON_val & _doc, const char * _member);

		OT_CORE_API_EXPORTONLY bool memberExists(OT_rJSON_val & _doc, const char * _member);

		// ##########################################################################################################
		
		// Document Getter

		//! @brief Will return the string from the specified member in the provided document
		//! @param _doc The document
		//! @param _member The name of the member
		//! @throw std::exception if the member is missing or the member type is not String
		OT_CORE_API_EXPORTONLY std::string getString(OT_rJSON_doc & _doc, const char * _member);

		//! @brief Will return the boolean value from the specified member in the provided document
		//! @param _doc The document
		//! @param _member The name of the member
		//! @throw std::exception if the member is missing or the member type is not boolean
		OT_CORE_API_EXPORTONLY bool getBool(OT_rJSON_doc & _doc, const char * _member);

		//! @brief Will return the integer value from the specified member in the provided document
		//! @param _doc The document
		//! @param _member The name of the member
		//! @throw std::exception if the member is missing or the member type is not integer
		OT_CORE_API_EXPORTONLY int getInt(OT_rJSON_doc & _doc, const char * _member);

		//! @brief Will return the unsigned integer value from the specified member in the provided document
		//! @param _doc The document
		//! @param _member The name of the member
		//! @throw std::exception if the member is missing or the member type is not unsigned integer
		OT_CORE_API_EXPORTONLY unsigned int getUInt(OT_rJSON_doc & _doc, const char * _member);

		//! @brief Will return the uLongLong from the specified member in the provided document
		//! @param _doc The document
		//! @param _member The name of the member
		//! @throw std::exception if the member is missing or the member type is not uLongLong
		OT_CORE_API_EXPORTONLY unsigned long long getULongLong(OT_rJSON_doc & _doc, const char * _member);

		//! @brief Will return the float from the specified member in the provided document
		//! @param _doc The document
		//! @param _member The name of the member
		//! @throw std::exception if the member is missing or the member type is not float
		OT_CORE_API_EXPORTONLY float getFloat(OT_rJSON_doc & _doc, const char * _member);

		//! @brief Will return the double from the specified member in the provided document
		//! @param _doc The document
		//! @param _member The name of the member
		//! @throw std::exception if the member is missing or the member type is not double
		OT_CORE_API_EXPORTONLY double getDouble(OT_rJSON_doc & _doc, const char * _member);

		OT_CORE_API_EXPORTONLY OT_rJSON_val getObject(OT_rJSON_doc& _doc, const char* _member);

		//! @brief Will return the uLongLong list from the specified member in the provided document
		//! @param _doc The document
		//! @param _member The name of the member
		//! @throw std::exception if the member is missing or the member type is not uLongLong
		OT_CORE_API_EXPORTONLY std::list<unsigned long long> getULongLongList(OT_rJSON_doc & _doc, const char * _member);

		//! @brief Will return the string list from the specified member in the provided document
		//! @param _doc The document
		//! @param _member The name of the member
		//! @throw std::exception if the member is missing or the member type is not array
		OT_CORE_API_EXPORTONLY std::list<std::string> getStringList(OT_rJSON_doc & _doc, const char * _member);

		//! @brief Will return the object list from the specified member in the provided document
		//! @param _doc The document
		//! @param _member The name of the member
		//! @throw std::exception if the member is missing or the member type is not array
		OT_CORE_API_EXPORTONLY std::list<std::string> getObjectList(OT_rJSON_doc & _doc, const char * _member);
		
		//! @brief Will return the object list from the specified member in the provided document
		//! @param _doc The document
		//! @param _member The name of the member
		//! @throw std::exception if the member is missing or the member type is not array
		OT_CORE_API_EXPORTONLY std::vector<double> getDoubleVector(OT_rJSON_doc & _doc, const char * _member);

		//! @brief Will return the object list from the specified member in the provided document
		//! @param _doc The document
		//! @param _member The name of the member
		//! @throw std::exception if the member is missing or the member type is not array
		OT_CORE_API_EXPORTONLY std::vector<int> getIntVector(OT_rJSON_doc & _doc, const char * _member);

		// ##########################################################################################################
		
		// Document Setter

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		OT_CORE_API_EXPORTONLY void add(OT_rJSON_doc & _doc, const std::string & _member, const char * _value);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		OT_CORE_API_EXPORTONLY void add(OT_rJSON_doc & _doc, const std::string & _member, const std::string & _value);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		OT_CORE_API_EXPORTONLY void add(OT_rJSON_doc & _doc, const std::string & _member, bool _value);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		OT_CORE_API_EXPORTONLY void add(OT_rJSON_doc & _doc, const std::string & _member, int _value);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		OT_CORE_API_EXPORTONLY void add(OT_rJSON_doc & _doc, const std::string & _member, unsigned int _value);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		OT_CORE_API_EXPORTONLY void add(OT_rJSON_doc & _doc, const std::string & _member, float _value);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		OT_CORE_API_EXPORTONLY void add(OT_rJSON_doc & _doc, const std::string & _member, double _value);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		OT_CORE_API_EXPORTONLY void add(OT_rJSON_doc & _doc, const std::string & _member, unsigned long long _value);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		OT_CORE_API_EXPORTONLY void add(OT_rJSON_doc & _doc, const std::string & _member, const double *_values, int size);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		OT_CORE_API_EXPORTONLY void add(OT_rJSON_doc & _doc, const std::string & _member, const std::list<std::string> &list);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		OT_CORE_API_EXPORTONLY void add(OT_rJSON_doc & _doc, const std::string & _member, const std::list<unsigned long long> &list);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		OT_CORE_API_EXPORTONLY void add(OT_rJSON_doc & _doc, const std::string & _member, const std::list<bool> &list);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		OT_CORE_API_EXPORTONLY void add(OT_rJSON_doc & _doc, const std::string & _member, OT_rJSON_val & _value);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		OT_CORE_API_EXPORTONLY void add(OT_rJSON_doc & _doc, const std::string & _member, const std::vector<double> &array);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		OT_CORE_API_EXPORTONLY void add(OT_rJSON_doc & _doc, const std::string & _member, const std::vector<int> &array);

		// ##########################################################################################################
		
		// Value Getter

		//! @brief Will return the string from the specified member in the provided document
		//! @param _doc The document
		//! @param _member The name of the member
		//! @throw std::exception if the member is missing or the member type is not String
		OT_CORE_API_EXPORTONLY std::string getString(OT_rJSON_val & _doc, const char * _member);

		//! @brief Will return the boolean value from the specified member in the provided document
		//! @param _doc The document
		//! @param _member The name of the member
		//! @throw std::exception if the member is missing or the member type is not boolean
		OT_CORE_API_EXPORTONLY bool getBool(OT_rJSON_val & _doc, const char * _member);

		//! @brief Will return the integer value from the specified member in the provided document
		//! @param _doc The document
		//! @param _member The name of the member
		//! @throw std::exception if the member is missing or the member type is not integer
		OT_CORE_API_EXPORTONLY int getInt(OT_rJSON_val & _doc, const char * _member);

		//! @brief Will return the unsigned integer value from the specified member in the provided document
		//! @param _doc The document
		//! @param _member The name of the member
		//! @throw std::exception if the member is missing or the member type is not unsigned integer
		OT_CORE_API_EXPORTONLY unsigned int getUInt(OT_rJSON_val & _doc, const char * _member);

		//! @brief Will return the uLongLong from the specified member in the provided document
		//! @param _doc The document
		//! @param _member The name of the member
		//! @throw std::exception if the member is missing or the member type is not uLongLong
		OT_CORE_API_EXPORTONLY unsigned long long getULongLong(OT_rJSON_val & _doc, const char * _member);

		//! @brief Will return the float from the specified member in the provided document
		//! @param _doc The document
		//! @param _member The name of the member
		//! @throw std::exception if the member is missing or the member type is not float
		OT_CORE_API_EXPORTONLY float getFloat(OT_rJSON_val & _doc, const char * _member);

		//! @brief Will return the double from the specified member in the provided document
		//! @param _doc The document
		//! @param _member The name of the member
		//! @throw std::exception if the member is missing or the member type is not double
		OT_CORE_API_EXPORTONLY double getDouble(OT_rJSON_val & _doc, const char * _member);

		//! @brief Will return the uLongLong list from the specified member in the provided document
		//! @param _doc The document
		//! @param _member The name of the member
		//! @throw std::exception if the member is missing or the member type is not uLongLong
		OT_CORE_API_EXPORTONLY std::list<unsigned long long> getULongLongList(OT_rJSON_val & _doc, const char * _member);

		//! @brief Will return the string list from the specified member in the provided document
		//! @param _doc The document
		//! @param _member The name of the member
		//! @throw std::exception if the member is missing or the member type is not uLongLong
		OT_CORE_API_EXPORTONLY std::list<std::string> getStringList(OT_rJSON_val & _doc, const char * _member);

		//! @brief Will return the object list from the specified member in the provided document
		//! @param _doc The document
		//! @param _member The name of the member
		//! @throw std::exception if the member is missing or the member type is not array
		OT_CORE_API_EXPORTONLY std::list<std::string> getObjectList(OT_rJSON_val & _doc, const char * _member);

		// ##########################################################################################################
		
		// Value Setter

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		OT_CORE_API_EXPORTONLY void add(OT_rJSON_doc & _doc, OT_rJSON_val & _val, const std::string & _member, const char * _value);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		OT_CORE_API_EXPORTONLY void add(OT_rJSON_doc & _doc, OT_rJSON_val & _val, const std::string & _member, const std::string & _value);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		OT_CORE_API_EXPORTONLY void add(OT_rJSON_doc & _doc, OT_rJSON_val & _val, const std::string & _member, bool _value);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		OT_CORE_API_EXPORTONLY void add(OT_rJSON_doc & _doc, OT_rJSON_val & _val, const std::string & _member, int _value);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		OT_CORE_API_EXPORTONLY void add(OT_rJSON_doc & _doc, OT_rJSON_val & _val, const std::string & _member, unsigned int _value);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		OT_CORE_API_EXPORTONLY void add(OT_rJSON_doc & _doc, OT_rJSON_val & _val, const std::string & _member, float _value);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		OT_CORE_API_EXPORTONLY void add(OT_rJSON_doc & _doc, OT_rJSON_val & _val, const std::string & _member, double _value);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		OT_CORE_API_EXPORTONLY void add(OT_rJSON_doc & _doc, OT_rJSON_val & _val, const std::string & _member, unsigned long long _value);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		OT_CORE_API_EXPORTONLY void add(OT_rJSON_doc & _doc, OT_rJSON_val & _val, const std::string & _member, const double *_values, int size);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		OT_CORE_API_EXPORTONLY void add(OT_rJSON_doc & _doc, OT_rJSON_val & _val, const std::string & _member, const std::list<std::string> &list);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		OT_CORE_API_EXPORTONLY void add(OT_rJSON_doc & _doc, OT_rJSON_val & _val, const std::string & _member, const std::list<unsigned long long> &list);

		//! @brief Will add the provided member and value to the provided document
		//! @param _doc The document
		//! @param _member The name of the member to add
		//! @param _value The value to set to the member
		OT_CORE_API_EXPORTONLY void add(OT_rJSON_doc & _doc, OT_rJSON_val & _val, const std::string & _member, OT_rJSON_val &_value);

		// ##########################################################################################################
		
		// Convert

		//! @brief Will return the JSON string created from the provided document
		//! @param _doc The document
		OT_CORE_API_EXPORTONLY std::string toJSON(OT_rJSON_doc & _doc);

		//! @brief Will return the JSON string created from the provided document
		//! @param _doc The document
		OT_CORE_API_EXPORTONLY std::string toJSON(OT_rJSON_val & _doc);


		OT_CORE_API_EXPORTONLY OT_rJSON_doc fromJSON(const std::string& jsonString);
	}
}
