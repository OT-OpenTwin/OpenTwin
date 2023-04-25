#pragma once
#include "MetadataParameter.h"

class MetadataParameterBundle
{
public:
	void AddMetadataParameter(MetadataParameter<std::string>& parameter) { _stringParameter.push_back(parameter); };
	void AddMetadataParameter(MetadataParameter<double>& parameter) { _doubleParameter.push_back(parameter); };
	void AddMetadataParameter(MetadataParameter<int32_t>& parameter) { _int32Parameter.push_back(parameter); };
	void AddMetadataParameter(MetadataParameter<int64_t>& parameter) { _int64Parameter.push_back(parameter); };
	
	std::list<MetadataParameter<std::string>>& getStringParameter() { return _stringParameter; }
	std::list<MetadataParameter<double>>& getDoubleParameter()  { return _doubleParameter; };
	std::list<MetadataParameter<int32_t>>& getInt32Parameter() { return _int32Parameter; };
	std::list<MetadataParameter<int64_t>>& getInt64Parameter() { return _int64Parameter; };

private:
	std::list<MetadataParameter<std::string>> _stringParameter;
	std::list<MetadataParameter<double>> _doubleParameter;
	std::list<MetadataParameter<int32_t>> _int32Parameter;
	std::list<MetadataParameter<int64_t>> _int64Parameter;
};

