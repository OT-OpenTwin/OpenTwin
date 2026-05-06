// @otlicense

// OpenTwin header
#include "OTCore/MetadataHandle/MetadataQuantity.h"

MetadataQuantity& MetadataQuantity::operator=(const MetadataQuantity& _other)
{
	MetadataQuantity temp(_other);
	swap(*this, temp);
	return *this;
}

MetadataQuantity& MetadataQuantity::operator=(MetadataQuantity&& _other) noexcept
{
	MetadataQuantity temp;
	swap(*this, _other);
	return *this;
}

MetadataQuantity::MetadataQuantity(const MetadataQuantity& _other)
	: quantityName(_other.quantityName),
	quantityIndex(_other.quantityIndex),
	quantityLabel(_other.quantityLabel),
	dataDimensions(_other.dataDimensions),
	dependingParameterIds(_other.dependingParameterIds),
	dependingParameterLabels(_other.dependingParameterLabels),
	m_tupleDescription(_other.m_tupleDescription)
{
	metaData.CopyFrom(_other.metaData, metaData.GetAllocator());
}

MetadataQuantity::MetadataQuantity(MetadataQuantity&& _other) noexcept
{
	swap(*this, _other);
}

void MetadataQuantity::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Label", ot::JsonString(quantityLabel, _allocator), _allocator);
	_object.AddMember("Name", ot::JsonString(quantityName, _allocator), _allocator);
	_object.AddMember("DependingParametersIDs", ot::JsonArray(dependingParameterIds, _allocator), _allocator);
	_object.AddMember("DependingParametersLabels", ot::JsonArray(dependingParameterLabels, _allocator), _allocator);
	ot::JsonObject entry;
	m_tupleDescription.addToJsonObject(entry, _allocator);
	_object.AddMember("TupleDescription", entry, _allocator);
	_object.AddMember("Dimensions", ot::JsonArray(dataDimensions, _allocator), _allocator);
}

void MetadataQuantity::setFromJsonObject(const ot::ConstJsonObject& _object) {
	quantityLabel = ot::json::getString(_object, "Label");
	quantityName = ot::json::getString(_object, "Name");
	//This one is not set in the state the serialised metadata is used in the pipeline.
	if (ot::json::exists(_object, "DependingParameterIDs")) {
		dependingParameterIds = ot::json::getUInt64Vector(_object, "DependingParameterIDs");
	}

	dependingParameterLabels = ot::json::getStringVector(_object, "DependingParametersLabels");
	auto allValueDesriptions = ot::json::getArray(_object, "ValueDescriptions");
	if (ot::json::exists(_object, "TupleDescription"))
	{
		auto tupleDescriptionObject = ot::json::getObject(_object, "TupleDescription");
		m_tupleDescription.setFromJsonObject(tupleDescriptionObject);
	}
	dataDimensions = ot::json::getUIntVector(_object, "Dimensions");
}

void MetadataQuantity::swap(MetadataQuantity& _a, MetadataQuantity& _b)
{
	std::swap(_a.quantityIndex, _b.quantityIndex);
	std::swap(_a.quantityName, _b.quantityName);
	std::swap(_a.quantityLabel, _b.quantityLabel);
	std::swap(_a.dataDimensions, _b.dataDimensions);
	std::swap(_a.dependingParameterLabels, _b.dependingParameterLabels);
	std::swap(_a.dependingParameterIds, _b.dependingParameterIds);
	std::swap(_a.m_tupleDescription, _b.m_tupleDescription);
	_a.metaData.Swap(_b.metaData);
}
