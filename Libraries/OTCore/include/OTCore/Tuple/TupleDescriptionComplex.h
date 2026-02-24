// @otlicense
#pragma once
#include "OTCore/Tuple/TupleDescription.h"
#include "OTCore/ComplexNumbers/ComplexNumberFormat.h"
#include "OTCore/TypeNames.h"
class OT_CORE_API_EXPORT TupleDescriptionComplex : public TupleDescription
{
	friend class TupleFactory;

public:
	TupleDescriptionComplex();

	TupleInstance createInstance(ot::ComplexNumberFormat _format, const std::vector<std::string>& _units, const std::vector<std::string>& _dataTypes = {ot::TypeNames::getDoubleTypeName(), ot::TypeNames::getDoubleTypeName()});
		
	std::string getName() const override { return m_name; }
private:
	inline static const std::string m_name = ot::ComplexNumbers::getTypeName();
	bool validateUnitsForFormat(const std::vector<std::string>& _units, const ot::ComplexNumberFormat& _format) const;
	bool validateDataTypesForFormat(const std::vector<std::string>& _dataTypes, const ot::ComplexNumberFormat& _format) const;
};