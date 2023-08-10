#include "gtest/gtest.h"
#include "OpenTwinCore/SimpleFactory.h"
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

TEST(SimpleFactoryTests, CreateValid)
{
	EXPECT_TRUE(ot::SimpleFactory::instance().contains("A"));
	EXPECT_NO_THROW(ot::SimpleFactory::instance().create("A"));
}

TEST(SimpleFactoryTests, CreateInvalid)
{
	EXPECT_ANY_THROW(ot::SimpleFactory::instance().create("C"));
}
