#pragma once
#include <string>
#include <map>
#include <list>
#include <MetadataParameter.h>
#include <MetadataQuantity.h>

#include "OTCore/Serializable.h"

class __declspec(dllexport) MetadataSeries : public ot::Serializable
{
public:
	MetadataSeries(const std::string& name):m_name(name){};
	MetadataSeries() = default;

	void addParameter(MetadataParameter& parameter) { m_parameter.push_back(parameter); }
	void addParameter(MetadataParameter&& parameter) { m_parameter.push_back(std::move(parameter)); }
	void addParameterReference(ot::UID _parameterUID) { m_parameterReferences.push_back(_parameterUID); m_parameterReferences.unique(); };
	
	void addQuantity(MetadataQuantity& quantity) { m_quantity.push_back(quantity); }
	void addQuantityReference(ot::UID _quantityUID) { m_quantityReferences.push_back(_quantityUID); m_quantityReferences.unique(); };
	
	void addMetadata(std::shared_ptr<MetadataEntry> metadata) { m_metaData[metadata->getEntryName()] = metadata; }
	const std::list<MetadataParameter>& getParameter() const { return m_parameter; }
	const std::list<MetadataQuantity>& getQuantities() const { return m_quantity; }
	const std::map <std::string, std::shared_ptr<MetadataEntry>> getMetadata()const { return m_metaData; }

	const std::string getName()const { return m_name; }
	const std::string getLabel()const { return m_label; }
	const uint64_t getSeriesIndex() const { return m_index; }
	
	//! @brief Is done by the Extender when a series is added to a campaign
	//! @param _index 
	void setIndex(uint64_t _index) { m_index = _index; };
	static const std::string getFieldName() { return "Series"; }

	void setLabel(const std::string& _label)
	{
		m_label = _label;
	}

	virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const
	{
		_object.AddMember("Label", ot::JsonString(m_label, _allocator), _allocator);
		_object.AddMember("Name", ot::JsonString(m_name, _allocator), _allocator);

		ot::JsonArray allQuantities;
		for (const MetadataQuantity& quantity : m_quantity)
		{

			ot::JsonObject object;
			quantity.addToJsonObject(object, _allocator);

			ot::JsonArray dependingParameterLabels;
			for (ot::UID dependingParameter : quantity.dependingParameterIds)
			{
				for (const MetadataParameter& param : m_parameter)
				{
					if (param.parameterUID == dependingParameter)
					{
						dependingParameterLabels.PushBack(ot::JsonString(param.parameterLabel, _allocator), _allocator);
					}
				}
			}
			object.RemoveMember("DependingParametersIDs");
			object.RemoveMember("DependingParametersLabels");
			object.AddMember("DependingParametersLabels", dependingParameterLabels, _allocator);

			allQuantities.PushBack(object, _allocator);
		}
		_object.AddMember("quantities", allQuantities, _allocator);

		ot::JsonArray allParameter;
		for (const MetadataParameter& parameter : m_parameter)
		{
			ot::JsonObject object;
			parameter.addToJsonObject(object, _allocator);
			allParameter.PushBack(object, _allocator);
		}
		_object.AddMember("parameter", allParameter, _allocator);

	}

	virtual void setFromJsonObject(const ot::ConstJsonObject& _object)
	{
		m_label = ot::json::getString(_object, "Label");
		m_name = ot::json::getString(_object, "Name");
		
		ot::ConstJsonArray allParameter = ot::json::getArray(_object, "parameter");
		for (rapidjson::SizeType i = 0; i < allParameter.Size(); i ++)
		{
			MetadataParameter parameter;
			parameter.setFromJsonObject(ot::json::getObject(allParameter, i));
			m_parameter.push_back(parameter);
		}

		ot::ConstJsonArray allQuantity = ot::json::getArray(_object, "quantities");
		for (rapidjson::SizeType i = 0; i < allQuantity.Size(); i++)
		{
			MetadataQuantity quantity;
			quantity.setFromJsonObject(ot::json::getObject(allQuantity, i));
			m_quantity.push_back(quantity);
		}
	}

private:
	std::string m_name;
	std::string m_label;
	ot::UID m_index = 0;

	std::list<MetadataParameter> m_parameter;
	ot::UIDList m_parameterReferences;
	std::list<MetadataQuantity> m_quantity;
	ot::UIDList m_quantityReferences;

	std::map <std::string, std::shared_ptr<MetadataEntry>> m_metaData;
};