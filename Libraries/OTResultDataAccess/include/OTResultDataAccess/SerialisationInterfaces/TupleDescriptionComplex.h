// @otlicense
#pragma once

#include "OTResultDataAccess/SerialisationInterfaces/TupleDescription.h"
#include "OTCore/ComplexNumbers/ComplexNumberFormat.h"
class TupleDescriptionComplex : public TupleDescription
{
public:
	TupleDescriptionComplex(ot::ComplexNumberFormat _format);
	ot::ComplexNumberFormat getTupleFormat() const { return m_tupleFormat; }

private:
	ot::ComplexNumberFormat m_tupleFormat;
	void setTupleElements();
	void initialiseTupleElementNames() override;
};