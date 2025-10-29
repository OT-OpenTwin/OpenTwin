// @otlicense

#include "OTCore/TextEncoding.h"
#include <codecvt>


std::string ot::TextEncoding::getString(ot::TextEncoding::EncodingStandard type) const
{
	if (_encodingStandardToString.find(type) == _encodingStandardToString.end())
	{
		throw std::exception("Encoding standard not found");
	}
	else
	{
		return (*_encodingStandardToString.find(type)).second;
	}
}

ot::TextEncoding::EncodingStandard ot::TextEncoding::getType(const std::string& type) const
{
	if (_stringToEncodingStandard.find(type) == _stringToEncodingStandard.end())
	{
		throw std::exception("Encoding standard not found");
	}
	else
	{
		return (*_stringToEncodingStandard.find(type)).second;
	}
}
