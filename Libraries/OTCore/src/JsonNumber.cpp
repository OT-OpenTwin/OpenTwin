//! @file JsonNumber.cpp
//! @author Alexander Kuester (alexk95)
//! @date June 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/JSONNumber.h"

ot::JsonNumber::JsonNumber(int _value) : 
	JsonValue(rapidjson::kNumberType) 
{ 
	this->SetInt64(static_cast<long>(_value)); 
}

ot::JsonNumber::JsonNumber(long _value) : 
	JsonValue(rapidjson::kNumberType) 
{ 
	this->SetInt64(_value); 
}

ot::JsonNumber::JsonNumber(long long _value) : 
	JsonValue(rapidjson::kNumberType) 
{ 
	this->SetInt64(_value); 
}

ot::JsonNumber::JsonNumber(unsigned long _value) : 
	JsonValue(rapidjson::kNumberType)
{ 
	this->SetUint64(_value); 
}

ot::JsonNumber::JsonNumber(unsigned long long _value) :
	JsonValue(rapidjson::kNumberType) 
{
	this->SetUint64(_value);
}

ot::JsonNumber::JsonNumber(float _value) : 
	JsonValue(rapidjson::kNumberType) 
{ 
	this->SetFloat(_value);
}

ot::JsonNumber::JsonNumber(double _value) : 
	JsonValue(rapidjson::kNumberType) 
{
	this->SetDouble(_value);
}