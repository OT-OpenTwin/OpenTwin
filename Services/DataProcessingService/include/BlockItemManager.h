#pragma once
#include <map>
#include "OTGui/GraphicsEditorPackage.h"

class BlockItemManager
{
public:
	void OrderUIToCreateBlockPicker();

private:
	ot::GraphicsEditorPackage* BuildUpBlockPicker();
};
