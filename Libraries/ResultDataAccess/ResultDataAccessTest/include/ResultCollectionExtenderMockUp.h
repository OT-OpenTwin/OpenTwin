#pragma once
#include "ResultCollectionExtender.h"
#include <list>
class ResultCollectionExtenderMockUp : public ResultCollectionExtender
{
	friend class FixtureResultCollectionExtender;
	ResultCollectionExtenderMockUp()
	{
		m_indices  = { 31,32,33,34, 35, 36,37,38,39,40 };
	}
private:
	std::list<uint64_t> m_indices;
	uint64_t getLastIndex()
	{
		uint64_t index = m_indices.back();
		m_indices.pop_back();
		return index;
	}
	const uint64_t findNextFreeSeriesIndex() override { return getLastIndex();	};
	const uint64_t findNextFreeQuantityIndex() override { return getLastIndex(); };
	const uint64_t findNextFreeParameterIndex() override { return getLastIndex(); };
};
