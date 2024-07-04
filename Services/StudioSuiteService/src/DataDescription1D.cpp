#include "DataDescription1D.h"
#include <assert.h>

DataDescription1D::~DataDescription1D()
{
	if (m_quantityDescription != nullptr)
	{
		delete m_quantityDescription;
		m_quantityDescription = nullptr;
	}
}

DataDescription1D::DataDescription1D()
{

}

DataDescription1D& DataDescription1D::operator=(DataDescription1D&& other) noexcept
{
	this->m_parameters = std::move(other.m_parameters);
	this->m_quantityDescription = other.m_quantityDescription;
	other.m_quantityDescription = nullptr;
	this->m_xAxisparameter = std::move(other.m_xAxisparameter);
	other.m_xAxisparameter = nullptr;
	return *this;
}

DataDescription1D::DataDescription1D(DataDescription1D&& other) noexcept
: m_parameters ( std::move(other.m_parameters)), m_quantityDescription (std::move(other.m_quantityDescription)), m_xAxisparameter (std::move(other.m_xAxisparameter))
{
	other.m_quantityDescription = nullptr;
	other.m_xAxisparameter = nullptr;
}

void DataDescription1D::setQuantityDescription(QuantityDescription* _quantityDescription)
{
	m_quantityDescription = _quantityDescription;
}

void DataDescription1D::setXAxisParameterDescription(std::shared_ptr<ParameterDescription> _parameterDescription)
{
	assert(_parameterDescription != nullptr);
	m_xAxisparameter = _parameterDescription;
}

void DataDescription1D::addParameterDescription(std::shared_ptr<ParameterDescription> _parameterDescription)
{
	assert(_parameterDescription != nullptr);
	m_parameters.push_back(_parameterDescription);
}

void DataDescription1D::addParameterDescriptions(std::list<std::shared_ptr<ParameterDescription>>::iterator _parameterDescriptionFirst, std::list<std::shared_ptr<ParameterDescription>>::iterator _parameterDescriptionEnd)
{
	m_parameters.insert(m_parameters.end(), _parameterDescriptionFirst, _parameterDescriptionEnd);
}
