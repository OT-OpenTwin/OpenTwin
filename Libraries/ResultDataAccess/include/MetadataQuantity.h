#pragma once
#include <stdint.h>
#include <string>
#include <map>
#include <list>
#include <vector>
#include <memory>

#include "OTCore/Variable.h"
#include "MetadataEntry.h"

struct __declspec(dllexport) MetadataQuantityValueDescription
{
	/****************** Are set by the campaign handling class *********************/
	ot::UID quantityIndex = 0;
	
	//Label which is unique in a series. In case that the unique name is already taken, the name is extended with a number.
	std::string quantityValueLabel = "";
	/******************************************************************************/

	std::string quantityValueName = "";


	std::string dataTypeName = "";

	std::string unit = "";
	//Attention: The result collection extender relies heavily on the logic of this method. Changes should be considered in the method for the creation of a new series.
	const bool operator==(const MetadataQuantityValueDescription& other) const
	{
		const bool isEqual = this->quantityValueName == other.quantityValueName &&
			this->unit == other.unit &&
			this->dataTypeName == other.dataTypeName;
		return isEqual;
	}
};

struct __declspec(dllexport) MetadataQuantity
{
	std::string quantityName = "";

	/****************** Is set by the campaign handling class *********************/
	
	//Identical with the first value description ID
	ot::UID quantityIndex = 0;
	
	//Label which is unique in a series. In case that the unique name is already taken, the name is extended with a number.
	std::string quantityLabel = "";
	/******************************************************************************/

	
	//E.g. {3,3} for a 3x3 Matrix
	std::vector<uint32_t> dataDimensions;

	std::vector<ot::UID> dependingParameterIds;
	std::list<MetadataQuantityValueDescription> valueDescriptions;

	std::map < std::string, std::shared_ptr<MetadataEntry>> metaData;

	/*************************** Quantity Container Database field key ***************************************/
	static const std::string getFieldName() { return "Quantity"; }
	/********************************************************************************************************/
	
	const bool operator==(const MetadataQuantity& _other) const
	{
		//Two metadata quantities are equal, if they have the same data structure:
		const bool isEqual  = this->dataDimensions == _other.dataDimensions &&
			this->valueDescriptions == _other.valueDescriptions;
		return isEqual;
	}
	const bool operator!=(const MetadataQuantity& _other) const
	{
		return !(*this == _other);
	}
};