// @otlicense

#pragma once
#include <string>
namespace BlockEntities
{
	struct __declspec(dllexport) SharedResources
	{
		static std::string getCornerImagePath() { return "Images/"; }
		static std::string getCornerImageNameDB() {return "DataBase.svg";}
		static std::string getCornerImageNameVis() { return "Visualisation.svg"; }
		static std::string getCornerImageNamePython() { return "Python.svg"; }
	};
}
