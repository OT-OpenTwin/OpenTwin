
#include "OTSystem/UrlEncoding.h"

#include <sstream>
#include <regex>
#include <iomanip>

std::string ot::url::urlEncode(std::string _str)
{
	std::ostringstream oss;
	std::regex r("[!'\\(\\)*-.0-9A-Za-z_~]");

	for (char& c : _str)
	{
		std::string s;
		s.push_back(c);

		if (std::regex_match(s, r))
		{
			oss << c;
		}
		else
		{
			oss << "%" << std::uppercase << std::setw(2) << std::setfill('0') << std::hex << (int)(c & 0xff);
		}
	}
	return oss.str();
}
