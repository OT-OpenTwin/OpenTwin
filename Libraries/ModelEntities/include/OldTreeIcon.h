#pragma once
#pragma warning(disable : 4251)

#include "OTCore/JSON.h"
#include "OTCore/CoreTypes.h"

OT_DECL_DEPRECATED("Use Gui/NavigationTreeIcon instead") class __declspec(dllexport) OldTreeIcon
{
public:
	OldTreeIcon() :
		size(0)
	{};

	int size;
	std::string visibleIcon;
	std::string hiddenIcon;

	void addToJsonDoc(ot::JsonDocument& doc);
};
