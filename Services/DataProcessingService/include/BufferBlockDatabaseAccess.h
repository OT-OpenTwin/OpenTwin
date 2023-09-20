#pragma once
#include <string>
#include <vector>
struct BufferBlockDatabaseAccess
{
	std::string SelectedMSMD;
	std::string SelectedProject;
	std::string SelectedQueryDimension;
	std::string SelectedQuantity1;
	std::string SelectedParameter1;

	std::vector<std::string> QuantityNames;
	std::vector<std::string> ParameterNames;
	std::vector<std::string> MeasurementMetadataNames;
};
