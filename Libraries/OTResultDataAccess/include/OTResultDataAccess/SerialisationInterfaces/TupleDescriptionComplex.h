// @otlicense
#pragma once

#include "OTResultDataAccess/SerialisationInterfaces/TupleDescription.h"
#include "OTCore/ComplexNumbers/ComplexNumberFormat.h"
class TupleDescriptionComplex : public TupleDescription
{
	friend class TupleFactory;

public:
	TupleDescriptionComplex() = default;
	TupleDescriptionComplex(ot::ComplexNumberFormat _format);
	void setComplexNumberFormat(ot::ComplexNumberFormat _format);
	ot::ComplexNumberFormat getTupleFormat() const { return m_tupleFormat; }

	std::string getName() const override { return m_name; }
	virtual TupleDescription* clone() override { return new TupleDescriptionComplex(*this); }
private:
	inline static const std::string m_name = "Complex";
	ot::ComplexNumberFormat m_tupleFormat;
	void setTupleElements();
	void initialiseTupleSpecifics() override;
};