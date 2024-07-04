#pragma once
#include <string>
#include <stdint.h>
#include <vector>

#include "OTCore/Variable.h"
#include "SParameterMatrixHelper.h"

struct QuantityDescription
{
	QuantityDescription() = default;
	QuantityDescription& operator=(const QuantityDescription& other) = default;
	QuantityDescription& operator=(QuantityDescription&& other) = default;
	QuantityDescription(const QuantityDescription& other) = default;
	QuantityDescription(QuantityDescription&& other) = default;
	virtual ~QuantityDescription() {}

	std::string quantityName = "";
	std::string quantityLabel = "";
	std::string quantityUnit = "";

	uint32_t quantityDataNumberOfRows = 0;
	uint32_t quantityDataNumberOfColumns = 0;
};

struct QuantityDescriptionSParameter : public QuantityDescription
{
	std::vector<SParameterMatrixHelper> quantityValues;
};

struct QuantityDescriptionCurve : public QuantityDescription
{
	QuantityDescriptionCurve()
	{
		quantityDataNumberOfRows = 1;
		quantityDataNumberOfColumns = 1;
	}
	std::vector<ot::Variable> quantityValues;
};