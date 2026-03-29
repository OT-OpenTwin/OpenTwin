// @otlicense

#pragma once

// OpenTwin header
#include "OTModelEntities/EntityPropertiesItems.h"

template <typename TContainer>
void EntityPropertiesSelection::resetOptionsImpl(const TContainer& _options)
{
	// Clear and reserve space
	m_options.clear();	
	m_options.reserve(_options.size());
	
	// Add options
	for (const auto& item : _options)
	{
		this->addOption(item);
	}
	m_options.shrink_to_fit();

	// Check whether the current value is still valid and reset to default if not
	if (m_options.empty())
	{
		m_value = "";
	}
	else
	{
		m_value = m_options.front();
	}

}