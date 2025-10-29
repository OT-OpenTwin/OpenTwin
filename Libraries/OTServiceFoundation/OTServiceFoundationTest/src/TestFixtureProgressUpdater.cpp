// @otlicense

#include <gtest/gtest.h>
#include "FixtureProgressUpdater.h"


TEST_F(FixtureProgressUpdater, TriggerFrequency_NbUpdatesGTNbSteps)
{
	ProgressUpdater& updater = getUpdater();
	updater.setTotalNumberOfUpdates(5, 3);
	EXPECT_EQ(getTriggerFrequency(), 1);
}

TEST_F(FixtureProgressUpdater, TriggerFrequency_NbUpdatesLTNbSteps)
{
	ProgressUpdater& updater = getUpdater();
	updater.setTotalNumberOfUpdates(2, 5);
	EXPECT_EQ(getTriggerFrequency(), 2);
}