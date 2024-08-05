//#include <gtest/gtest.h>
//#include "OptionSettings.h"
//
//TEST(TouchstoneOptionSettings, DefaultValues)
//{
//	ts::OptionSettings settings;
//	EXPECT_EQ(settings.getFormat(), ts::option::Format::magnitude_angle);
//	EXPECT_EQ(settings.getFrequency(), ts::option::Frequency::GHz);
//	EXPECT_EQ(settings.getParameter(), ts::option::Parameter::Scattering);
//	EXPECT_EQ(settings.getReferenceResistance(), ot::Variable(50));
//}
//
//
//TEST(TouchstoneOptionSettings, Equal)
//{
//	ts::OptionSettings settings1(ts::option::Frequency::Hz,ts::option::Format::real_imaginary,ts::option::Parameter::Hybrid_h,ot::Variable(70));
//	ts::OptionSettings settings2(ts::option::Frequency::Hz,ts::option::Format::real_imaginary,ts::option::Parameter::Hybrid_h,ot::Variable(70));
//	EXPECT_EQ(settings1, settings2);
//}
//
//TEST(TouchstoneOptionSettings, NotEqual)
//{
//	ts::OptionSettings settings1(ts::option::Frequency::Hz, ts::option::Format::real_imaginary, ts::option::Parameter::Hybrid_h, ot::Variable(70));
//	ts::OptionSettings settings2;
//	EXPECT_FALSE(settings1 == settings2);
//}