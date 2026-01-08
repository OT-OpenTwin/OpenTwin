// @otlicense
#pragma once

#include <vector>
#include <string>

namespace ResultCollectionDefaultIndexes
{
	const static std::vector<std::string>& getDefaultIndexes()
	{
		static std::vector<std::string> m_defaultIndexes = { "Quantity" ,"Series" };
		return m_defaultIndexes;
	}
}
