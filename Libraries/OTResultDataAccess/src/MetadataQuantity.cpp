// @otlicense

// OpenTwin header
#include "OTResultDataAccess/MetadataHandle/MetadataQuantity.h"

void MetadataQuantityValueDescription::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Label", ot::JsonString(quantityValueLabel, _allocator), _allocator);
	_object.AddMember("Name", ot::JsonString(quantityValueName, _allocator), _allocator);
	_object.AddMember("Type", ot::JsonString(dataTypeName, _allocator), _allocator);
	_object.AddMember("Unit", ot::JsonString(unit, _allocator), _allocator);
}

void MetadataQuantityValueDescription::setFromJsonObject(const ot::ConstJsonObject& _object) {
	quantityValueLabel = ot::json::getString(_object, "Label");
	quantityValueName = ot::json::getString(_object, "Name");
	unit = ot::json::getString(_object, "Unit");
	dataTypeName = ot::json::getString(_object, "Type");
}

void MetadataQuantity::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Label", ot::JsonString(quantityLabel, _allocator), _allocator);
	_object.AddMember("Name", ot::JsonString(quantityName, _allocator), _allocator);
	_object.AddMember("DependingParametersIDs", ot::JsonArray(dependingParameterIds, _allocator), _allocator);
	_object.AddMember("DependingParametersLabels", ot::JsonArray(dependingParameterLabels, _allocator), _allocator);
	ot::JsonArray allValueDescriptions;
	for (auto& valueDescription : valueDescriptions) {
		ot::JsonObject entry;
		valueDescription.addToJsonObject(entry, _allocator);
		allValueDescriptions.PushBack(entry, _allocator);
	}
	_object.AddMember("ValueDescriptions", allValueDescriptions, _allocator);
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
	for (rapidjson::SizeType i = 0; i < allValueDesriptions.Size(); i++) {
		auto entry = ot::json::getObject(allValueDesriptions, i);
		MetadataQuantityValueDescription valueDescription;
		valueDescription.setFromJsonObject(entry);
		valueDescriptions.push_back(valueDescription);
	}
	dataDimensions = ot::json::getUIntVector(_object, "Dimensions");
}