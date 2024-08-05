#pragma once
#include <string>
#include <map>
#include <list>
#include <MetadataParameter.h>
#include <MetadataQuantity.h>

class __declspec(dllexport) MetadataSeries
{
public:
	MetadataSeries(const std::string& name):m_name(name){};
	
	void addParameter(MetadataParameter& parameter) { m_parameter.push_back(parameter); }
	void addParameterReference(ot::UID _parameterUID) { m_parameterReferences.push_back(_parameterUID); m_parameterReferences.unique(); };
	
	void addQuantity(MetadataQuantity& quantity) { m_quantity.push_back(quantity); }
	void addQuantityReference(ot::UID _quantityUID) { m_quantityReferences.push_back(_quantityUID); m_quantityReferences.unique(); };
	
	void addMetadata(std::shared_ptr<MetadataEntry> metadata) { m_metaData[metadata->getEntryName()] = metadata; }
	const std::list<MetadataParameter>& getParameter() const { return m_parameter; }
	const std::list<MetadataQuantity>& getQuantities() const { return m_quantity; }
	const std::map <std::string, std::shared_ptr<MetadataEntry>> getMetadata()const { return m_metaData; }

	const std::string getName()const { return m_name; }
	const uint64_t getSeriesIndex() const { return m_index; }
	
	//! @brief Is done by the Extender when a series is added to a campaign
	//! @param _index 
	void setIndex(uint64_t _index) { m_index = _index; };
	static const std::string getFieldName() { return "Series"; }

	void setLabel(const std::string& _label)
	{
		m_label = _label;
	}

private:
	const std::string m_name;
	std::string m_label;
	ot::UID m_index = 0;

	std::list<MetadataParameter> m_parameter;
	ot::UIDList m_parameterReferences;
	std::list<MetadataQuantity> m_quantity;
	ot::UIDList m_quantityReferences;

	std::map <std::string, std::shared_ptr<MetadataEntry>> m_metaData;
};