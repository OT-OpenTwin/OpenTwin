#pragma once
#include <string>
#include <vector>
struct BufferBlockDatabaseAccess
{
	std::string SelectedMSMD;
	std::string SelectedProject;

	std::vector<std::string> QuantityNames;
	std::vector<std::string> ParameterNames;
	std::vector<std::string> MeasurementMetadataNames;
};
