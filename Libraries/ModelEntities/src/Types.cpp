#include "stdafx.h"

#include "Types.h"
#include "OTCommunication/ActionTypes.h"

void TreeIcon::addToJsonDoc(ot::JsonDocument& doc)
{
	doc.AddMember(OT_ACTION_PARAM_UI_TREE_IconSize, size, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_TREE_IconItemVisible, ot::JsonString(visibleIcon, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_TREE_IconItemHidden, ot::JsonString(hiddenIcon, doc.GetAllocator()), doc.GetAllocator());
}


