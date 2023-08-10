#include <iostream>
#include "gtest/gtest.h"

#include <string>

#include "OTGui/GraphicsLayoutItemCfg.h"
#include "OpenTwinCore/rJSON.h"
#include "OpenTwinCore/SimpleFactory.h"

TEST(GraphicsLayoutITemCfgTest, Serialize)
{
	// Create a test configuration
	ot::GraphicsVBoxLayoutItemCfg* root = new ot::GraphicsVBoxLayoutItemCfg;
	ot::GraphicsHBoxLayoutItemCfg* one = new ot::GraphicsHBoxLayoutItemCfg;
	ot::GraphicsGridLayoutItemCfg* two = new ot::GraphicsGridLayoutItemCfg;

	EXPECT_NO_THROW(ot::SimpleFactoryRegistrar<ot::GraphicsVBoxLayoutItemCfg>(OT_SimpleFactoryJsonKeyValue_GraphicsVBoxLayoutItemCfg));
	EXPECT_NO_THROW(ot::SimpleFactoryRegistrar<ot::GraphicsHBoxLayoutItemCfg>(OT_SimpleFactoryJsonKeyValue_GraphicsHBoxLayoutItemCfg));

	root->addChildItem(one);
	//root->addChildItem(two);

	// Export
	OT_rJSON_createDOC(doc);
	EXPECT_NO_THROW(root->addToJsonObject(doc, doc));
	
	// Import
	OT_rJSON_parseDOC(docI, ot::rJSON::toJSON(doc).c_str());
	EXPECT_TRUE(ot::rJSON::toJSON(doc) == ot::rJSON::toJSON(docI));
	
	// Factory
	ot::GraphicsLayoutItemCfg* rootI = nullptr;
	EXPECT_NO_THROW(rootI = ot::SimpleFactory::instance().createType<ot::GraphicsLayoutItemCfg>(docI));
	EXPECT_TRUE(rootI != nullptr);
	EXPECT_TRUE(rootI->simpleFactoryObjectKey() == root->simpleFactoryObjectKey());
	EXPECT_NO_THROW(rootI->setFromJsonObject(docI));

	// Export 2
	OT_rJSON_doc docE;
	EXPECT_NO_THROW(rootI->addToJsonObject(docE, docE));
	EXPECT_TRUE(ot::rJSON::toJSON(doc) == ot::rJSON::toJSON(docE));
}
