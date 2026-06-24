// @otlicense

// OpenTwin header
#include "OTCore/MetadataHandle/MetadataQuantity.h"
#include "OTCore/MetadataHandle/Helper.h"

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
	if (ot::json::exists(_object, "TupleDescription"))
	{
		auto tupleDescriptionObject = ot::json::getObject(_object, "TupleDescription");
		m_tupleDescription.setFromJsonObject(tupleDescriptionObject);
	}
	dataDimensions = ot::json::getUIntVector(_object, "Dimensions");
}

size_t MetadataQuantity::getMemSize()
{
	size_t total = sizeof(MetadataQuantity);

	// --- std::string heap allocations ---
	total += ot::stringHeapSize(quantityName);
	total += ot::stringHeapSize(quantityLabel);

	// --- std::vector<uint32_t> dataDimensions ---
	// capacity() gives the allocated element count; multiply by element size.
	// sizeof(vector) is already included via sizeof(MetadataQuantity).
	total += dataDimensions.capacity() * sizeof(uint32_t);

	// --- std::vector<std::string> dependingParameterLabels ---
	// Heap buffer for the vector elements themselves:
	total += dependingParameterLabels.capacity() * sizeof(std::string);
	// Plus the heap-allocated string data inside each element:
	for (const std::string& label : dependingParameterLabels)
	{
		total += ot::stringHeapSize(label);
	}

	// --- std::vector<ot::UID> dependingParameterIds ---
	total += dependingParameterIds.capacity() * sizeof(ot::UID);

	// --- ot::TupleInstance m_tupleDescription ---
	// sizeof(MetadataQuantity) already covers the in-object storage.
	// If TupleInstance owns heap data, delegate to its own getMemSize() if available:
	// total += m_tupleDescription.getMemSize() - sizeof(ot::TupleInstance);
	// Otherwise, leave it at zero additional heap bytes (conservative assumption).

	// --- ot::JsonDocument metaData ---
	total += metaData.GetAllocator().Capacity();

	return total;
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
