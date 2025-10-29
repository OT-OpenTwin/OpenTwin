// @otlicense

#pragma once
#include "FixtureVariable.h"

INSTANTIATE_TEST_CASE_P(TestAllVariableTypes, FixtureVariable, ::testing::Values(
	ot::Variable(3),
	ot::Variable(static_cast<int64_t>(3)),
	ot::Variable(3.),
	ot::Variable(3.f),
	ot::Variable("3"),
	ot::Variable(true)
));


