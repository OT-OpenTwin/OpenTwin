#pragma once
#include <string>
#include "OTCore/DefensiveProgramming.h"
#include "QuantityDescription.h"


class __declspec(dllexport)ValueFormatSetter
{
public:
	void setValueFormatRealOnly(QuantityDescription& quantityDescription,std::string _unit)
	{
		quantityDescription.clearValueDescriptions();
		quantityDescription.addValueDescription(m_realValueName, ot::TypeNames::getDoubleTypeName(), _unit);
		POST(quantityDescription.getMetadataQuantity().valueDescriptions.size() == 1);
	}

	void setValueFormatImaginaryOnly(QuantityDescription& quantityDescription, std::string _unit)
	{
		quantityDescription.clearValueDescriptions();
		quantityDescription.addValueDescription(m_imagValueName, ot::TypeNames::getDoubleTypeName(), _unit);
		POST(quantityDescription.getMetadataQuantity().valueDescriptions.size() == 1);
	}

	void setValueFormatRealImaginary(QuantityDescription& quantityDescription, std::string _unit)
	{
		quantityDescription.clearValueDescriptions();
		//Don't change order of addValueDescription!
		quantityDescription.addValueDescription(m_realValueName, ot::TypeNames::getDoubleTypeName(), _unit);
		quantityDescription.addValueDescription(m_imagValueName, ot::TypeNames::getDoubleTypeName(), _unit);
		POST(quantityDescription.getMetadataQuantity().valueDescriptions.size() == 2);
	}

	void setValueFormatDecibelPhase(QuantityDescription& quantityDescription)
	{
		quantityDescription.clearValueDescriptions();
		//Don't change order of addValueDescription!
		quantityDescription.addValueDescription(m_decibelValueName, ot::TypeNames::getDoubleTypeName(), m_decibleUnitName);
		quantityDescription.addValueDescription(m_phaseValueName, ot::TypeNames::getDoubleTypeName(), m_phaseUnitName);
		POST(quantityDescription.getMetadataQuantity().valueDescriptions.size() == 2);
	}

	void setValueFormatMagnitudePhase(QuantityDescription& quantityDescription)
	{
		quantityDescription.clearValueDescriptions();
		//Don't change order of addValueDescription!
		quantityDescription.addValueDescription(m_magnitudeValueName, ot::TypeNames::getDoubleTypeName(), "");
		quantityDescription.addValueDescription(m_phaseValueName, ot::TypeNames::getDoubleTypeName(), m_phaseUnitName);
		POST(quantityDescription.getMetadataQuantity().valueDescriptions.size() == 2);
	}

private:

	bool invariant() { return true; }
	const std::string m_realValueName = "Real";
	const std::string m_imagValueName = "Imaginary";
	const std::string m_phaseValueName = "Phase";
	const std::string m_magnitudeValueName = "Magnitude";
	const std::string m_decibelValueName = "Decibel";

	const std::string m_decibleUnitName = "dB";
	const std::string m_phaseUnitName = "Deg";
};
