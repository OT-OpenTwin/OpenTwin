#pragma once
#pragma warning(disable : 4251)

//#include <OpenTwinCommunication/actionTypes.h>
#include "OTCore/rJSON.h"
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

	void addToJsonDoc(rapidjson::Document *doc);
};
