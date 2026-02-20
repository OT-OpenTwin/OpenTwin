// @otlicense

// OpenTwin header
#include "OTResultDataAccess/MetadataHandle/MetadataQuantity.h"

void MetadataQuantity::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Label", ot::JsonString(quantityLabel, _allocator), _allocator);
	_object.AddMember("Name", ot::JsonString(quantityName, _allocator), _allocator);
	_object.AddMember("DependingParametersIDs", ot::JsonArray(dependingParameterIds, _allocator), _allocator);
	_object.AddMember("DependingParametersLabels", ot::JsonArray(dependingParameterLabels, _allocator), _allocator);
	ot::JsonObject entry;
	m_tupleDescription->addToJsonObject(entry, _allocator);
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
		m_tupleDescription->setFromJsonObject(tupleDescriptionObject);
	}
	dataDimensions = ot::json::getUIntVector(_object, "Dimensions");
}