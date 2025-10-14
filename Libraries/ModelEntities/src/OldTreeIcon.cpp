#include "stdafx.h"

#include "OldTreeIcon.h"
#include "OTCommunication/ActionTypes.h"

void OldTreeIcon::addToJsonDoc(ot::JsonDocument& doc) const
{
	doc.AddMember(OT_ACTION_PARAM_UI_TREE_IconSize, size, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_TREE_IconItemVisible, ot::JsonString(visibleIcon, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_TREE_IconItemHidden, ot::JsonString(hiddenIcon, doc.GetAllocator()), doc.GetAllocator());
}


