#pragma once
#include <memory>
#include <list>
#include <string>

#include "ParameterDescription.h"
#include "QuantityDescription.h"

class DataDescription1D
{
public:
	virtual ~DataDescription1D();
	DataDescription1D();
	DataDescription1D& operator=(DataDescription1D&& other) noexcept;
	DataDescription1D(DataDescription1D&& other) noexcept;
	DataDescription1D& operator=(const DataDescription1D& other) = delete;
	DataDescription1D(const DataDescription1D& other) = delete;

	const std::list<std::shared_ptr<ParameterDescription>>&  getParameters() const { return m_parameters; }
	std::shared_ptr<ParameterDescription> getXAxisParameter() const { return m_xAxisparameter; }
	QuantityDescription* getQuantity() { return m_quantityDescription; }

	void setQuantityDescription(QuantityDescription* _quantityDescription);
	void setXAxisParameterDescription(std::shared_ptr<ParameterDescription> _parameterDescription);
	void addParameterDescription(std::shared_ptr<ParameterDescription> _parameterDescription);
	void addParameterDescriptions(std::list<std::shared_ptr<ParameterDescription>>::iterator _parameterDescriptionFirst, std::list<std::shared_ptr<ParameterDescription>>::iterator _parameterDescriptionEnd);
private:

	std::list<std::shared_ptr<ParameterDescription>> m_parameters;
	std::shared_ptr<ParameterDescription> m_xAxisparameter = nullptr;
	QuantityDescription* m_quantityDescription = nullptr;
};


