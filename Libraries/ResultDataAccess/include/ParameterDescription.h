#pragma once
#include <list>

#include "MetadataParameter.h"
#include "OTCore/Variable.h"

//! @brief Acts as a container for the parameter data during the assembly. 
class __declspec(dllexport) ParameterDescription
{
public:
	ParameterDescription(const MetadataParameter& _metadataParameter)
		:m_metadataParameter(_metadataParameter){}
	ParameterDescription(MetadataParameter&& _metadataParameter)
		: m_metadataParameter(std::move(_metadataParameter)){}

	MetadataParameter& getMetadataParameter() 
	{ 
		if (m_parameterValues.size() != m_metadataParameter.values.size())
		{
			m_parameterValues = m_metadataParameter.values;
		}
		return m_metadataParameter; 
	}
private:
	MetadataParameter m_metadataParameter;
	std::list<ot::Variable> m_parameterValues; //Nicht nötig, wenn die Parameter Value Liste nicht optimiert wird.
};
