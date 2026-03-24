#pragma once
//@otlicense

#include "OTCore/CoreAPIExport.h"
#include <list>
#include <string>

class  OT_CORE_API_EXPORT RegexHelper
{
public:
	static void applyRegexFilter(std::list<std::string>& _options, const std::string& _filter);
};
