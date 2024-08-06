#pragma once

#include <list>
#include <memory>

#include "QuantityDescription.h"
#include "ParameterDescription.h"

class __declspec(dllexport) DatasetDescription
{
public:
	virtual ~DatasetDescription();
	DatasetDescription();
	DatasetDescription& operator=(DatasetDescription&& other) noexcept;
	DatasetDescription(DatasetDescription&& other) noexcept;
	DatasetDescription& operator=(const DatasetDescription& other) = delete;
	DatasetDescription(const DatasetDescription& other) = delete;

	const std::list<std::shared_ptr<MetadataParameter>>& getParameters() const { return m_parameters; }
	QuantityDescription* getQuantityDescription() { return m_quantityDescription; }

	void setQuantityDescription(QuantityDescription* _quantityDescription);
	
	void addParameterDescription(std::shared_ptr<MetadataParameter> _parameterDescription);
	void addParameterDescriptions(std::list<std::shared_ptr<MetadataParameter>>::iterator _parameterDescriptionFirst, std::list<std::shared_ptr<MetadataParameter>>::iterator _parameterDescriptionEnd);
protected:

	std::list<std::shared_ptr<MetadataParameter>> m_parameters;
	QuantityDescription* m_quantityDescription = nullptr;
};
