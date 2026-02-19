#include "OTResultDataAccess/SerialisationInterfaces/TupleDescriptionComplex.h"

TupleDescriptionComplex::TupleDescriptionComplex(ot::ComplexNumberFormat _format)
	: m_tupleFormat(_format)
{
	m_name = "Complex";
	m_formatName = ot::ComplexNumbers::getFormatString(m_tupleFormat);
	setTupleElements();
}

void TupleDescriptionComplex::setTupleElements()
{
	if (m_tupleFormat == ot::ComplexNumberFormat::Cartesian)
	{
		m_tupleElementNames = { "Real", "Imaginary" };
	}
	else if (m_tupleFormat == ot::ComplexNumberFormat::Polar)
	{
		m_tupleElementNames = { "Magnitude", "Phase" };
	}
}

void TupleDescriptionComplex::initialiseTupleElementNames()
{
	m_tupleFormat = ot::ComplexNumbers::getFormatFromString(m_formatName);
	setTupleElements();
}
