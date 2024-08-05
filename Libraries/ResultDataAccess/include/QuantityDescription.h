#pragma once
#include <string>
#include <stdint.h>
#include <vector>

#include "OTCore/Variable.h"
#include "OTCore/CoreTypes.h"

#include "SParameterMatrixHelper.h"
#include "MetadataQuantity.h"
#include "OTCore/DefensiveProgramming.h"

class __declspec(dllexport) QuantityDescription
{
public:
	QuantityDescription() = default;
	QuantityDescription& operator=(const QuantityDescription& _other) = default;
	QuantityDescription& operator=(QuantityDescription&& _other) noexcept
	{
		m_metadataQuantity = std::move(_other.m_metadataQuantity);
		return *this;
	};
	QuantityDescription(const QuantityDescription& _other) = default;
	QuantityDescription(QuantityDescription&& _other) noexcept
		: m_metadataQuantity(std::move(_other.m_metadataQuantity))
	{}
	virtual ~QuantityDescription() {}
	
	void setValueFormatRealOnly(std::string _unit)
	{
		m_metadataQuantity.valueDescriptions.clear();
		addValueDescription(m_realValueName, ot::TypeNames::getDoubleTypeName(), _unit);
		POST(m_metadataQuantity.valueDescriptions.size() == 1);
	}

	void setValueFormatRealImaginary(std::string _unit)
	{
		m_metadataQuantity.valueDescriptions.clear();
		//Don't change order of addValueDescription!
		addValueDescription(m_realValueName, ot::TypeNames::getDoubleTypeName(), _unit);
		addValueDescription(m_imagValueName, ot::TypeNames::getDoubleTypeName(), _unit);
		POST(m_metadataQuantity.valueDescriptions.size() == 2);
	}

	void setValueFormatDecibelPhase()
	{
		m_metadataQuantity.valueDescriptions.clear();
		//Don't change order of addValueDescription!
		addValueDescription(m_decibelValueName, ot::TypeNames::getDoubleTypeName(), m_decibleUnitName);
		addValueDescription(m_phaseValueName, ot::TypeNames::getDoubleTypeName(), m_phaseUnitName);
		POST(m_metadataQuantity.valueDescriptions.size() == 2);
	}

	void setValueFormatMagnitudePhase()
	{
		m_metadataQuantity.valueDescriptions.clear();
		//Don't change order of addValueDescription!
		addValueDescription(m_magnitudeValueName, ot::TypeNames::getDoubleTypeName(), "");
		addValueDescription(m_phaseValueName, ot::TypeNames::getDoubleTypeName(), m_phaseUnitName);
		POST(m_metadataQuantity.valueDescriptions.size() == 2);
	}
	
	void setName(const std::string& _quantityName)
	{
		m_metadataQuantity.quantityName = _quantityName;
	}
	const std::string& getName() const { return m_metadataQuantity.quantityName; }
	 
	MetadataQuantity& getMetadataQuantity() { return m_metadataQuantity; }
	void setDependingParameterUIDs(const std::list<ot::UID>& _dependingParameterUIDs)
	{
		m_metadataQuantity.dependingParameterIds = { _dependingParameterUIDs.begin(), _dependingParameterUIDs.end() };
	}
	
	//! @brief Depending on the chosen value format, the first value either a real value, the magnitude or the decible value
	ot::UID getFirstValueQuantityIndex() const
	{
		auto& descriptions = m_metadataQuantity.valueDescriptions;
		PRE(descriptions.size() >= 1 && descriptions.begin()->quantityIndex != 0);
		return descriptions.begin()->quantityIndex;
	}

	//! @brief Depending on the chosen value format, the first value either a real value, the magnitude or the decible value
	ot::UID getSecondValueQuantityIndex() const
	{
		auto& descriptions = m_metadataQuantity.valueDescriptions;
		PRE(descriptions.size() >= 2 && (descriptions.begin()++)->quantityIndex != 0);
		auto secondDescription = descriptions.begin()++;
		return secondDescription->quantityIndex;
	}
protected:

	void addValueDescription(const std::string _valueName, const std::string _valueDataType, const std::string _valueUnit)
	{
		MetadataQuantityValueDescription valueDescription;
		valueDescription.quantityValueName = _valueName;
		valueDescription.unit = _valueUnit;
		valueDescription.dataTypeName = _valueDataType;
		m_metadataQuantity.valueDescriptions.push_back(std::move(valueDescription));
	}

	inline bool invariant() const
	{
		return true;
	}

	MetadataQuantity m_metadataQuantity;	

private:
	const std::string m_realValueName = "Real";
	const std::string m_imagValueName = "Imaginary";
	const std::string m_phaseValueName = "Phase";
	const std::string m_magnitudeValueName = "Magnitude";
	const std::string m_decibelValueName = "Decibel";

	const std::string m_decibleUnitName = "dB";
	const std::string m_phaseUnitName = "Deg";

	std::string m_selectedValueName;
};