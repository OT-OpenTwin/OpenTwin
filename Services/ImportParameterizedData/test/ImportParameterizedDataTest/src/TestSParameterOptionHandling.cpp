#include "FixtureSParameterHandler.h"
#include "OptionSettings.h"
#include "Options.h"
#include <sstream>

TEST_F(FixtureSParameterHandler, OptionDetectionSuccess)
{
	const sp::OptionSettings expectedOptionSettings(sp::option::Frequency::kHz, sp::option::Format::real_imaginary, sp::option::Parameter::Admittance, ot::Variable(100));
	
	std::string setting = "# kHz Y RI R 100";
	const sp::OptionSettings actualSettings	= GetOptionSettings(setting);
	

	EXPECT_TRUE(expectedOptionSettings == actualSettings);
}

TEST_F(FixtureSParameterHandler, OptionFrequencyOmitted)
{
	const sp::OptionSettings expectedOptionSettings(sp::option::Frequency::GHz, sp::option::Format::real_imaginary, sp::option::Parameter::Admittance, ot::Variable(100));

	std::string setting = "# Y RI R 100";
	const sp::OptionSettings actualSettings = GetOptionSettings(setting);


	EXPECT_TRUE(expectedOptionSettings == actualSettings);
}

TEST_F(FixtureSParameterHandler, OptionFormatOmitted)
{
	const sp::OptionSettings expectedOptionSettings(sp::option::Frequency::kHz, sp::option::Format::magnitude_angle, sp::option::Parameter::Admittance, ot::Variable(100));

	std::string setting = "# kHz Y R 100";
	const sp::OptionSettings actualSettings = GetOptionSettings(setting);


	EXPECT_TRUE(expectedOptionSettings == actualSettings);
}

TEST_F(FixtureSParameterHandler, OptionParameterOmitted)
{
	const sp::OptionSettings expectedOptionSettings(sp::option::Frequency::kHz, sp::option::Format::real_imaginary, sp::option::Parameter::Scattering, ot::Variable(100));

	std::string setting = "# kHz S RI R 100";
	const sp::OptionSettings actualSettings = GetOptionSettings(setting);


	EXPECT_TRUE(expectedOptionSettings == actualSettings);
}

TEST_F(FixtureSParameterHandler, OptionResistanceOmitted)
{
	const sp::OptionSettings expectedOptionSettings(sp::option::Frequency::kHz, sp::option::Format::real_imaginary, sp::option::Parameter::Admittance, ot::Variable(50));

	std::string setting = "# kHz Y RI";
	const sp::OptionSettings actualSettings = GetOptionSettings(setting);


	EXPECT_TRUE(expectedOptionSettings == actualSettings);
}

TEST_F(FixtureSParameterHandler, BlankOptionLine)
{
	const sp::OptionSettings expectedOptionSettings;

	std::string setting = "#";
	const sp::OptionSettings actualSettings = GetOptionSettings(setting);


	EXPECT_TRUE(expectedOptionSettings == actualSettings);
}

TEST_F(FixtureSParameterHandler, OptionTwoEntries)
{
	const sp::OptionSettings expectedOptionSettings(sp::option::Frequency::kHz, sp::option::Format::real_imaginary, sp::option::Parameter::Scattering, ot::Variable(50));

	std::string setting = "# kHz RI";
	const sp::OptionSettings actualSettings = GetOptionSettings(setting);


	EXPECT_TRUE(expectedOptionSettings == actualSettings);
}

TEST_F(FixtureSParameterHandler, OptionOnlyResistance)
{
	const sp::OptionSettings expectedOptionSettings(sp::option::Frequency::GHz, sp::option::Format::magnitude_angle, sp::option::Parameter::Scattering, ot::Variable(80));

	std::string setting = "#R 80";
	const sp::OptionSettings actualSettings = GetOptionSettings(setting);


	EXPECT_TRUE(expectedOptionSettings == actualSettings);
}

TEST_F(FixtureSParameterHandler, OptionUnevenNumberOfWhitespaces)
{
	const sp::OptionSettings expectedOptionSettings(sp::option::Frequency::kHz, sp::option::Format::real_imaginary, sp::option::Parameter::Scattering, ot::Variable(50));

	std::string setting = "#          kHz      RI";
	const sp::OptionSettings actualSettings = GetOptionSettings(setting);


	EXPECT_TRUE(expectedOptionSettings == actualSettings);
}

TEST_F(FixtureSParameterHandler, OptionChangedOrder)
{
	const sp::OptionSettings expectedOptionSettings(sp::option::Frequency::kHz, sp::option::Format::real_imaginary, sp::option::Parameter::Scattering, ot::Variable(50));

	std::string setting = "# RI kHz";
	const sp::OptionSettings actualSettings = GetOptionSettings(setting);


	EXPECT_TRUE(expectedOptionSettings == actualSettings);
}


TEST_F(FixtureSParameterHandler, OptionDetectedAfterComments)
{
	const sp::OptionSettings expectedOptionSettings;

	std::string setting = "! This is a first comment.\n"
		"!And a second\n"
		"#";
	AnalyseFile(setting);
	const sp::OptionSettings actualSettings = GetOptionSettings(setting);

	EXPECT_TRUE(expectedOptionSettings == actualSettings);
}

TEST_F(FixtureSParameterHandler, CommentsDetected)
{
	const std::string expectedComments = " This is a first comment.\nAnd a second\nOne more\n";

	std::string setting = "! This is a first comment.\n"
		"!And a second\n"
		"#\n"
		"!One more\n";
	AnalyseFile(setting);

	const std::string actualComments = GetComments();

	EXPECT_TRUE(actualComments == expectedComments);
}