// @otlicense

#include <iostream>
#include "gtest/gtest.h"
/*
#include <string>

#include "OTGui/GraphicsVBoxLayoutItemCfg.h"
#include "OTGui/GraphicsHBoxLayoutItemCfg.h"
#include "OTGui/GraphicsGridLayoutItemCfg.h"
#include "OTCore/JSON.h"
#include "OTCore/SimpleFactory.h"

TEST(GraphicsLayoutITemCfgTest, Serialize)
{
	// Create a test configuration
	ot::GraphicsVBoxLayoutItemCfg* root = new ot::GraphicsVBoxLayoutItemCfg;
	ot::GraphicsHBoxLayoutItemCfg* one = new ot::GraphicsHBoxLayoutItemCfg;
	ot::GraphicsGridLayoutItemCfg* two = new ot::GraphicsGridLayoutItemCfg;

	EXPECT_NO_THROW(ot::SimpleFactoryRegistrar<ot::GraphicsVBoxLayoutItemCfg>(OT_SimpleFactoryJsonKeyValue_GraphicsVBoxLayoutItemCfg));
	EXPECT_NO_THROW(ot::SimpleFactoryRegistrar<ot::GraphicsHBoxLayoutItemCfg>(OT_SimpleFactoryJsonKeyValue_GraphicsHBoxLayoutItemCfg));

	root->addChildItem(one);
	
	// <<Export>>
	ot::JsonDocument oDoc;
	EXPECT_NO_THROW(root->addToJsonObject(oDoc, oDoc.GetAllocator()));
	std::string oStr = oDoc.toJson();
	EXPECT_FALSE(oStr.empty());

	// <<Import>>
	ot::JsonDocument iDoc;
	iDoc.Parse(oStr.c_str());
	std::string iStr = iDoc.toJson();
	EXPECT_TRUE(oStr == iStr);
	
	// <<Factory>>
	ot::GraphicsLayoutItemCfg* iRoot = nullptr;
	EXPECT_NO_THROW(iRoot = ot::SimpleFactory::instance().createType<ot::GraphicsLayoutItemCfg>(iDoc.GetObject()));
	EXPECT_TRUE(iRoot != nullptr);
	EXPECT_TRUE(iRoot->simpleFactoryObjectKey() == root->simpleFactoryObjectKey());
	EXPECT_NO_THROW(iRoot->setFromJsonObject(iDoc));

	// <<Export 2>>
	OT_rJSON_createDOC(rDoc);
	EXPECT_NO_THROW(iRoot->addToJsonObject(rDoc, rDoc));

	// Ensure the final export matches the original export
	std::string rStr = ot::rJSON::toJSON(rDoc);
	EXPECT_TRUE(iStr == rStr); // Original > Result
	EXPECT_FALSE(rStr.empty());
	
}
*/
