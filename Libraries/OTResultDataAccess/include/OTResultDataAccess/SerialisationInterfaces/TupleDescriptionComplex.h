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
		m_name = "Complex";
		if (m_tupleFormat == ComplexFormats::Real_Imaginary)
		{
			m_formatName = "Real/Imaginary";
			m_tupleElementNames = { "Real", "Imaginary" };
		}
		else if (m_tupleFormat == ComplexFormats::Magnitude_Phase)
		{
			m_formatName = "Magnitude/Phase";
			m_tupleElementNames = { "Magnitude", "Phase" };
		}
	}

	ComplexFormats getTupleFormat() const { return m_tupleFormat; }

private:
	ComplexFormats m_tupleFormat;
	void initialiseTupleElementNames() override
	{
		if(m_formatName == "Real/Imaginary")
		{
			m_tupleElementNames = { "Real", "Imaginary" };
			m_tupleFormat = ComplexFormats::Real_Imaginary;
		}
		else if (m_formatName == "Magnitude/Phase")
		{
			m_tupleElementNames = { "Magnitude", "Phase" };
			m_tupleFormat = ComplexFormats::Magnitude_Phase;
		}
	};
};