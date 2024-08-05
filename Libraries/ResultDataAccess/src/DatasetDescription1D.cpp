#include "DatasetDescription1D.h"

void DatasetDescription1D::setXAxisParameterDescription(std::shared_ptr<MetadataParameter> _parameterDescription)
{
	//assert(_parameterDescription != nullptr);
	m_parameters.push_front(_parameterDescription);
}
