#include "OTResultDataAccess/SerialisationInterfaces/TupleDescription.h"

class OT_RESULTDATAACCESS_API_EXPORT TupleDescriptionComplex : public TupleDescription
{
public:
	enum class ComplexFormats
	{
		Real_Imaginary,
		Magnitude_Phase
	};

	TupleDescriptionComplex(ComplexFormats _format)
		: m_tupleFormat(_format)
	{
		m_formatName = "Complex";
		m_tupleID = "C";

		if (_format == ComplexFormats::Real_Imaginary)
		{
			m_tupleID = "RI";
			m_formatName = "Real/Imaginary";
			m_tupleElementNames = { "Real", "Imaginary" };
		}
		else if (_format == ComplexFormats::Magnitude_Phase)
		{
			m_tupleID = "MP";
			m_formatName = "Magnitude/Phase";
			m_tupleElementNames = { "Magnitude", "Phase" };
		}
	}

	ComplexFormats getTupleFormat() const { return m_tupleFormat; }

private:
	ComplexFormats m_tupleFormat;
};