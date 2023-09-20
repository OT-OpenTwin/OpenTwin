#pragma once
#include <string>
#include <vector>
struct BufferBlockDatabaseAccess
{
	std::string SelectedProject;
	std::string SelectedMSMD;
	std::string SelectedQuantity;
	std::string SelectedParameter1;
	std::string SelectedParameter2;
	std::string SelectedParameter3;

	std::vector<std::string> QuantityNames;
	std::vector<std::string> ParameterNames;
	std::vector<std::string> MeasurementMetadataNames;
};
