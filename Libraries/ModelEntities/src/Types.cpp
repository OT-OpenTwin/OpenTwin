#include "stdafx.h"

#include "Types.h"
#include <OpenTwinCommunication/ActionTypes.h>

void TreeIcon::addToJsonDoc(rapidjson::Document *doc)
{
	ot::rJSON::add(*doc, OT_ACTION_PARAM_UI_TREE_IconSize, size);
	ot::rJSON::add(*doc, OT_ACTION_PARAM_UI_TREE_IconItemVisible, visibleIcon);
	ot::rJSON::add(*doc, OT_ACTION_PARAM_UI_TREE_IconItemHidden, hiddenIcon);
}


