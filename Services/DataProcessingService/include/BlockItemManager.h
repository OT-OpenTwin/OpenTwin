#pragma once
#include <map>
#include "OTGui/GraphicsEditorPackage.h"

class BlockItemManager
{
public:
	void OrderUIToCreateBlockPicker();
	std::string getEntityBlockName(const std::string& blockName);

private:
	ot::GraphicsEditorPackage* BuildUpBlockPicker();
	static std::map<std::string, std::string> _blockNameToEntityBlockName;
	//static std::map<std::string, std::string> _blockNameToDefaultConnectors;
};
