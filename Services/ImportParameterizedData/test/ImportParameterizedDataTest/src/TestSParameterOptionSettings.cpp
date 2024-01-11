#include <gtest/gtest.h>
#include "OptionSettings.h"

TEST(SParameterOptionSettings, DefaultValues)
{
	sp::OptionSettings settings;
	EXPECT_EQ(settings.getFormat(), sp::option::Format::magnitude_angle);
	EXPECT_EQ(settings.getFrequency(), sp::option::Frequency::GHz);
	EXPECT_EQ(settings.getParameter(), sp::option::Parameter::Scattering);
	EXPECT_EQ(settings.getReferenceResistance(), ot::Variable(50));
}


TEST(SParameterOptionSettings, Equal)
{
	sp::OptionSettings settings1(sp::option::Frequency::Hz,sp::option::Format::real_imaginary,sp::option::Parameter::Hybrid_h,ot::Variable(70));
	sp::OptionSettings settings2(sp::option::Frequency::Hz,sp::option::Format::real_imaginary,sp::option::Parameter::Hybrid_h,ot::Variable(70));
	EXPECT_EQ(settings1, settings2);
}

TEST(SParameterOptionSettings, NotEqual)
{
	sp::OptionSettings settings1(sp::option::Frequency::Hz, sp::option::Format::real_imaginary, sp::option::Parameter::Hybrid_h, ot::Variable(70));
	sp::OptionSettings settings2;
	EXPECT_FALSE(settings1 == settings2);
}