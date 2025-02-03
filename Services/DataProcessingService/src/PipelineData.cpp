#include "PipelineData.h"

void PipelineData::copyMetaDataReferences(PipelineData& _other)
{
	_other.m_parameter = this->m_parameter;
	_other.m_quantity = this->m_quantity;
	_other.m_quantityDescription = this->m_quantityDescription;
	_other.m_campaign = this->m_campaign;
	_other.m_series = this->m_series;
	_other.m_fixedParameter = this->m_fixedParameter;
}
