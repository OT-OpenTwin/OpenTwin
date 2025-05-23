#pragma once
#include "OTGui/GuiTypes.h"

struct VisualiserState
{
	bool m_selected;
	bool m_singleSelection;
	bool m_setFocus = true;
	bool m_anyVisualiserHasFocus = false;
	ot::SelectionOrigin m_selectionOrigin;
};
