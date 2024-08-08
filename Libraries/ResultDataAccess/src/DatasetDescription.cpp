#include "DatasetDescription.h"
#include <assert.h>

DatasetDescription::~DatasetDescription()
{
	if (m_quantityDescription != nullptr)
	{
		delete m_quantityDescription;
		m_quantityDescription = nullptr;
	}
}

DatasetDescription::DatasetDescription()
{

}

DatasetDescription& DatasetDescription::operator=(DatasetDescription&& other) noexcept
{
	this->m_parameters = std::move(other.m_parameters);
	this->m_quantityDescription = other.m_quantityDescription;
	other.m_quantityDescription = nullptr;
	return *this;
}

DatasetDescription::DatasetDescription(DatasetDescription&& other) noexcept
	: m_parameters(std::move(other.m_parameters)), m_quantityDescription(std::move(other.m_quantityDescription))
{
	other.m_quantityDescription = nullptr;
}

void DatasetDescription::setQuantityDescription(QuantityDescription* _quantityDescription)
{
	m_quantityDescription = _quantityDescription;
}

void DatasetDescription::addParameterDescription(std::shared_ptr<ParameterDescription> _parameterDescription)
{
	assert(_parameterDescription != nullptr);
	m_parameters.push_back(_parameterDescription);
}

void DatasetDescription::addParameterDescriptions(std::list<std::shared_ptr<ParameterDescription>>_parameterDescriptions)
{
	m_parameters.insert(m_parameters.end(), _parameterDescriptions.begin(), _parameterDescriptions.end());
}

