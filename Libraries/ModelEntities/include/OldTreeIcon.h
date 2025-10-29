// @otlicense

#pragma once
#pragma warning(disable : 4251)

#include "OTCore/JSON.h"
#include "OTCore/CoreTypes.h"

OT_DECL_DEPRECATED("Use Gui/NavigationTreeIcon instead") class __declspec(dllexport) OldTreeIcon
{
	OT_DECL_DEFCOPY(OldTreeIcon)
	OT_DECL_DEFMOVE(OldTreeIcon)
public:
	OldTreeIcon() :
		size(32)
	{};

	int size;
	std::string visibleIcon;
	std::string hiddenIcon;

	void addToJsonDoc(ot::JsonDocument& doc) const;
};
