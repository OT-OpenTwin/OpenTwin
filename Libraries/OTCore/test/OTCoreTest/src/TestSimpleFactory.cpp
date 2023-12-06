#include "gtest/gtest.h"
#include "OTCore/SimpleFactory.h"
#include <string>

class TesterA : public ot::SimpleFactoryObject {
public:
	virtual std::string simpleFactoryObjectKey(void) const { return "A"; };
};
class TesterB : public ot::SimpleFactoryObject {
public:
	virtual std::string simpleFactoryObjectKey(void) const { return "B"; };
};

TEST(RegistrarTests, Register)
{
	EXPECT_NO_THROW(ot::SimpleFactoryRegistrar<TesterA> regA("A"));
}

TEST(RegistrarTests, CheckExists)
{
	EXPECT_NO_THROW(ot::SimpleFactory::instance().contains("A"));
	EXPECT_TRUE(ot::SimpleFactory::instance().contains("A"));
}

//TEST(SimpleFactoryTests, CreateValid)
//{
//	EXPECT_TRUE(ot::SimpleFactory::instance().contains("A"));
//	EXPECT_NO_THROW(ot::SimpleFactory::instance().create("A"));
//}
//
//TEST(SimpleFactoryTests, CreateInvalid)
//{
//	EXPECT_TRUE(ot::SimpleFactory::instance().create("C") == nullptr);
//}
//
//TEST(SimpleFactoryTests, All)
//{
//	EXPECT_NO_THROW(ot::SimpleFactoryRegistrar<TesterB> regA("B"));
//	EXPECT_NO_THROW(ot::SimpleFactory::instance().create("B"));
//	EXPECT_TRUE(ot::SimpleFactory::instance().create("B")->simpleFactoryObjectKey() == "B");
//	EXPECT_TRUE(ot::SimpleFactory::instance().create("B") != nullptr);
//}
