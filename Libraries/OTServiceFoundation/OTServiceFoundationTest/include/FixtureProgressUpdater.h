// @otlicense

#pragma once
#include <gtest/gtest.h>
#include "OTServiceFoundation/ProgressUpdater.h"

class FixtureProgressUpdater : public testing::Test
{
public:
	FixtureProgressUpdater();
	uint64_t getTriggerFrequency() const;
	ProgressUpdater& getUpdater() { return m_updater; };

private:
	ProgressUpdater m_updater;
};
