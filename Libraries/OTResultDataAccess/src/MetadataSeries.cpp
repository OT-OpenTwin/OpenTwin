#include "OTResultDataAccess/MetadataHandle/MetadataSeries.h"

MetadataSeries::MetadataSeries(const MetadataSeries& _other)
	: m_name(_other.m_name), m_label(_other.m_label), m_index(_other.m_index),
	m_parameter(_other.m_parameter), m_parameterReferences(_other.m_parameterReferences),
	m_quantity(_other.m_quantity), m_quantityReferences(_other.m_quantityReferences)
{
	m_metaData.CopyFrom(_other.m_metaData, m_metaData.GetAllocator());
}

MetadataSeries& MetadataSeries::operator=(const MetadataSeries& _other)
{
	MetadataSeries temp (_other);
	swap(*this,temp);
	return *this;
}

MetadataSeries::MetadataSeries(MetadataSeries&& _other) noexcept
	:MetadataSeries()
{
	swap(*this, _other);
}

MetadataSeries& MetadataSeries::operator=(MetadataSeries&& _other)  noexcept
{
	MetadataSeries temp;
	swap(*this, _other);
	return *this;
}

void MetadataSeries::swap(MetadataSeries& _a, MetadataSeries& _b) noexcept
{
	std::swap(_a.m_name, _b.m_name);
	std::swap(_a.m_label, _b.m_label);
	std::swap(_a.m_index, _b.m_index);
	std::swap(_a.m_parameter, _b.m_parameter);
	std::swap(_a.m_parameterReferences, _b.m_parameterReferences);
	std::swap(_a.m_quantity, _b.m_quantity);
	std::swap(_a.m_quantityReferences, _b.m_quantityReferences);
	m_metaData.Swap(_b.m_metaData);	
}

void MetadataSeries::setMetadata(const ot::JsonDocument& _metadata)
{
	m_metaData.CopyFrom(_metadata, m_metaData.GetAllocator());
}

void MetadataSeries::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const
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

void MetadataSeries::setFromJsonObject(const ot::ConstJsonObject& _object)
{
	m_label = ot::json::getString(_object, "Label");
	m_name = ot::json::getString(_object, "Name");

	ot::ConstJsonArray allParameter = ot::json::getArray(_object, "parameter");
	for (rapidjson::SizeType i = 0; i < allParameter.Size(); i++)
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