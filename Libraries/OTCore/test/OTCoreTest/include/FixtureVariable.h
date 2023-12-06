#pragma once
#include "gtest/gtest.h"
#include "OTCore/Variable.h"

class FixtureVariable : public testing::TestWithParam<ot::Variable>
{

public:
	const std::list<ot::Variable>& GetExpectedEqualEntries()const { return _expectedEqualValues; };
	const std::list<ot::Variable>& GetExpectedLargerEntries()const { return _expectedLargerValues; };

private:
	std::list<ot::Variable> _expectedEqualValues{
	ot::Variable(3),
		ot::Variable(static_cast<int64_t>(3)),
		ot::Variable(3.),
		ot::Variable(3.f),
		ot::Variable("3"),
		ot::Variable(true)
	};
	std::list<ot::Variable> _expectedLargerValues{
	ot::Variable(5),
		ot::Variable(static_cast<int64_t>(5)),
		ot::Variable(5.),
		ot::Variable(5.f),
		ot::Variable("5"),
		ot::Variable(true)
	};
};
