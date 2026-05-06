// @otlicense

// OpenTwin header
#include "OTWidgets/Header/HeaderFilterState.h"

void ot::HeaderFilterState::clear()
{
	m_selectedFilters.clear();

	m_hoveredFilter = -1;
	m_pressedFilter = -1;
}

bool ot::HeaderFilterState::hasActiveFilter() const
{
	for (const auto& it : m_selectedFilters)
	{
		if (!it.second.isEmpty())
		{
			return true;
		}
	}

	return false;
}

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

std::map<int, QStringList> ot::HeaderFilterState::getActiveFilters() const
{
	std::map<int, QStringList> result;
	for (const auto& it : m_selectedFilters)
	{
		if (!it.second.isEmpty())
		{
			result.insert(result.end(), it);
		}
	}
	return result;
}
