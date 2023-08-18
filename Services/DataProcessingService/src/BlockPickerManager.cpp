#include "BlockPickerManager.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/LinearGradientPainter2D.h"
#include "OTGui/GraphicsCollectionCfg.h"
#include "OTGui/GraphicsEditorPackage.h"
#include "OTGui/GraphicsLayoutItemCfg.h"

#include "BlockDatabaseAccess.h"

ot::GraphicsEditorPackage* BlockPickerManager::BuildUpBlockPicker()
{
	ot::GraphicsEditorPackage* pckg = new ot::GraphicsEditorPackage("Data Processing", "Data Processing");
	ot::GraphicsCollectionCfg* controlBlockCollection = new ot::GraphicsCollectionCfg("Control Blocks", "Control Blocks");
	ot::GraphicsCollectionCfg* controlBlockDatabaseCollection = new ot::GraphicsCollectionCfg("Database", "Database");
	ot::GraphicsCollectionCfg* controlBlockVisualizationCollection = new ot::GraphicsCollectionCfg("Visualization", "Visualization");

	ot::GraphicsCollectionCfg* mathBlockCollection = new ot::GraphicsCollectionCfg("Mathematical Operations", "Mathematical Operations");
	ot::GraphicsCollectionCfg* customizedBlockCollection = new ot::GraphicsCollectionCfg("Customized Blocks", "Customized Blocks");

	controlBlockCollection->addChildCollection(controlBlockDatabaseCollection);
	controlBlockCollection->addChildCollection(controlBlockVisualizationCollection);

	BlockDatabaseAccess dbA("Hans");

	controlBlockDatabaseCollection->addItem(dbA.GetBlock());


	//a2->addItem(createTestBlock2("Alpha 3"));
	pckg->addCollection(controlBlockCollection);
	pckg->addCollection(customizedBlockCollection);
	pckg->addCollection(mathBlockCollection);


	return pckg;
}
