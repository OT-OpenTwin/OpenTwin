#pragma once
#include <string>
class EnvironmentsGarbageCollector
{
public:
	void removeEnvironmentsIfNecessary();

private:
	const int32_t m_daysUntilRemoval = 3;
	const int32_t m_numberOfEnvironmentsLimit = 5;
};
