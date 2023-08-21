#pragma once
#include "OTGui/GraphicsEditorPackage.h"

class BlockPickerManager
{
public:
	void OrderUIToCreateBlockPicker();
private:
	ot::GraphicsEditorPackage* BuildUpBlockPicker();
};
