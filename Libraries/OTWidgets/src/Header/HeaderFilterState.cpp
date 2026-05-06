// @otlicense

// OpenTwin header
#include "OTWidgets/Header/HeaderFilterState.h"

bool ot::HeaderFilterState::isFilterActive(int _index) const
{
	const auto it = m_selectedFilters.find(_index);
	if (it != m_selectedFilters.end())
	{
		return !it->second.isEmpty();
	}
	else
	{
		return false;
	}
}

std::list<int> ot::HeaderFilterState::getActiveFilters() const
{
	std::list<int> result;
	for (const auto& it : m_selectedFilters)
	{
		if (!it.second.isEmpty())
		{
			result.push_back(it.first);
		}
	}

	return result;
}

QStringList ot::HeaderFilterState::getFilter(int _index) const
{
	auto it = m_selectedFilters.find(_index);
	if (it != m_selectedFilters.end())
	{
		return it->second;
	}
	else
	{
		return QStringList();
	}
}
