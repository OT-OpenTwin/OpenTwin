#pragma once
#include <string>
#include <list>
#include <map>
struct BufferBlockDatabaseAccess
{
	std::string SelectedProject = "";
	std::string SelectedMSMD = "";
	std::string SelectedQuantity = "";
	std::string SelectedParameter1 = "";
	std::string SelectedParameter2 = "";
	std::string SelectedParameter3 = "";

	std::string dataTypeQuantity = "";
	std::string dataTypeParameter1 = "";
	std::string dataTypeParameter2 = "";
	std::string dataTypeParameter3 = "";

	std::string selectedDimension = "";

	std::list<std::string> QuantityNames;
	std::list<std::string> ParameterNames;
	std::list<std::string> MeasurementMetadataNames;
	std::map<std::string, std::string> dataTypesByName;
};
