// @otlicense

#pragma once
#include <list>

#include "MetadataParameter.h"
#include "OTCore/Variable.h"

//! @brief Acts as a container for the parameter data during the assembly. 
class __declspec(dllexport) ParameterDescription
{
public:
	ParameterDescription(const MetadataParameter& _metadataParameter, bool _constantForDataset)
		:m_metadataParameter(_metadataParameter), m_constantForDataset(_constantForDataset){}
	~ParameterDescription() = default;

	ParameterDescription(ParameterDescription&& _other) noexcept = default;
	ParameterDescription& operator=(ParameterDescription&& _other) = default;

	ParameterDescription(const ParameterDescription& _other) =delete;
	ParameterDescription& operator=(const ParameterDescription& _other) = delete;

	MetadataParameter& getMetadataParameter() 
	{ 
		return m_metadataParameter; 
	}

	void setParameterConstantForDataset(bool _constantForDataset) { m_constantForDataset = _constantForDataset; };
	bool getParameterConstantForDataset() const { return m_constantForDataset; }
private:
	MetadataParameter m_metadataParameter;
	bool m_constantForDataset = false;
};
