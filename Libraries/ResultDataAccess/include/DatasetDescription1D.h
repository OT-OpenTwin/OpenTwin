#pragma once
#include "DatasetDescription.h"

class __declspec(dllexport) DatasetDescription1D : public DatasetDescription
{
public:
	std::shared_ptr<MetadataParameter> getXAxisParameter() const { return m_parameters.front(); }
	void setXAxisParameterDescription(std::shared_ptr<MetadataParameter> _parameterDescription);
};
