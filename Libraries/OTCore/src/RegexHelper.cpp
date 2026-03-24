#include "OTCore/DataFilter/RegexHelper.h"
#include <regex>

void RegexHelper::applyRegexFilter(std::list<std::string>& _options, const std::string& _filter)
{
	std::regex pattern(_filter);

	auto option = _options.begin();
	while (option != _options.end())
	{
		if (!std::regex_match(*option, pattern))
		{
			option = _options.erase(option);
		}
		else
		{
			option++;
		}
	}
}
