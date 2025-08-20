#pragma once

// OpenTwin header
#include "OTGui/GuiTypes.h"
#include "OTWidgets/SelectionData.h"

// std header
#include <list>

class SceneNodeBase;

struct VisualiserState
{
	bool m_selected;
	bool m_singleSelection;
	bool m_setFocus = true;
	bool m_anyVisualiserHasFocus = false;
	ot::SelectionData m_selectionData; 
	std::list<SceneNodeBase*> m_selectedNodes;
};
