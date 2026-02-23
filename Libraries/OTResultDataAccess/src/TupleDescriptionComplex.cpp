#include "OTResultDataAccess/SerialisationInterfaces/TupleDescriptionComplex.h"


TupleDescriptionComplex::TupleDescriptionComplex(ot::ComplexNumberFormat _format)
{
	setComplexNumberFormat(_format);
}

void TupleDescriptionComplex::setComplexNumberFormat(ot::ComplexNumberFormat _format)
{
	m_tupleFormat = (_format);
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

	m_tupleFormatNames = {ot::ComplexNumbers::getFormatString(ot::ComplexNumberFormat::Cartesian), ot::ComplexNumbers::getFormatString(ot::ComplexNumberFormat::Polar) };
}

void TupleDescriptionComplex::initialiseTupleSpecifics()
{
	m_tupleFormat = ot::ComplexNumbers::getFormatFromString(m_formatName);
	setTupleElements();
}
