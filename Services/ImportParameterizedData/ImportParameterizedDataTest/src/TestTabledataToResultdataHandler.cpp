// @otlicense

#include "FixtureTabledataToResultdataHandler.h"


TEST_F(FixtureTabledataToResultdataHandler, UnitExtractionFromName_NoUnit)
{
	const std::string expectedName = "Frequency";
	std::string name = "Frequency";
	std::string unit =	extractUnitFromName(name);
	ASSERT_EQ(expectedName, name);
	ASSERT_EQ(unit, "");
}


TEST_F(FixtureTabledataToResultdataHandler, UnitExtractionFromName_WithUnit)
{
	const std::string expectedName = "Frequency";
	const std::string expectedUnit = "Hz";
	std::string name = "Frequency [Hz]";
	std::string unit = extractUnitFromName(name);
	ASSERT_EQ(expectedName, name);
	ASSERT_EQ(expectedUnit,unit);
}


TEST_F(FixtureTabledataToResultdataHandler, UnitExtractionFromName_WithoutUnitOpeningBracked)
{
	const std::string expectedName = "Frequency [Hz";
	const std::string expectedUnit = "";
	std::string name = "Frequency [Hz";
	std::string unit = extractUnitFromName(name);
	ASSERT_EQ(expectedName, name);
	ASSERT_EQ(expectedUnit, unit);
}

TEST_F(FixtureTabledataToResultdataHandler, UnitExtractionFromName_WithoutUnitClosingBracked)
{
	const std::string expectedName = "Frequency Hz]";
	const std::string expectedUnit = "";
	std::string name = "Frequency Hz]";
	std::string unit = extractUnitFromName(name);
	ASSERT_EQ(expectedName, name);
	ASSERT_EQ(expectedUnit, unit);
}