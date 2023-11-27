#pragma once
#pragma warning(disable : 4251)

#include "OTCore/JSON.h"
#include "OTCore/CoreTypes.h"

class __declspec(dllexport) TreeIcon
{
public:
	TreeIcon() :
		size(0)
	{};

	int size;
	std::string visibleIcon;
	std::string hiddenIcon;

	void addToJsonDoc(ot::JsonDocument& doc);
};
