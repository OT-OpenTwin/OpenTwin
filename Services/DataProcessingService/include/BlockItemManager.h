#pragma once
#include <map>
#include "OTGui/GraphicsPackage.h"

class BlockItemManager
{
public:
	void OrderUIToCreateBlockPicker();

private:
	ot::GraphicsItemPickerPackage* BuildUpBlockPicker();
};
