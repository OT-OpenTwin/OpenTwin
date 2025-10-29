// @otlicense

//! @file Path2D.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTGui/Path2D.h"

ot::Path2D::Path2D() {}

ot::Path2D::Path2D(const Point2D& _startPos) : m_pos(_startPos) {}

ot::Path2D::Path2D(const Path2D& _other) 
	: m_pos(_other.m_pos), m_entries(_other.m_entries)
{}

ot::Path2D::~Path2D() {}

ot::Path2D& ot::Path2D::operator = (const Path2D& _other) {
	m_pos = _other.m_pos;
	m_entries = _other.m_entries;

	return *this;
}

void ot::Path2D::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	JsonObject posObj;
	m_pos.addToJsonObject(posObj, _allocator);
	_object.AddMember("Pos", posObj, _allocator);

	JsonArray entryArr;
	for (const PathEntry& e : m_entries) {
		JsonObject entryObj;
		entryObj.AddMember("Type", JsonString(toString(e.type), _allocator), _allocator);

		JsonObject startObj;
		e.start.addToJsonObject(startObj, _allocator);
		entryObj.AddMember("Start", startObj, _allocator);

		JsonObject stopObj;
		e.stop.addToJsonObject(stopObj, _allocator);
		entryObj.AddMember("Stop", stopObj, _allocator);

		if (e.type == BerzierType) {
			JsonObject c1Obj;
			e.control1.addToJsonObject(c1Obj, _allocator);
			entryObj.AddMember("C1", c1Obj, _allocator);

			JsonObject c2Obj;
			e.control2.addToJsonObject(c2Obj, _allocator);
			entryObj.AddMember("C2", c2Obj, _allocator);
		}

		entryArr.PushBack(entryObj, _allocator);
	}

	_object.AddMember("Entries", entryArr, _allocator);
}

void ot::Path2D::setFromJsonObject(const ot::ConstJsonObject& _object) {
	this->clear();

	ConstJsonObject posObj = json::getObject(_object, "Pos");
	m_pos.setFromJsonObject(posObj);

	ConstJsonArray entryArr = json::getArray(_object, "Entries");
	for (JsonSizeType i = 0; i < entryArr.Size(); i++) {
		ConstJsonObject entryObj = json::getObject(entryArr, i);

		PathEntry newEntry;
		newEntry.type = toPathEntryType(json::getString(entryObj, "Type"));
		newEntry.start.setFromJsonObject(json::getObject(entryObj, "Start"));
		newEntry.stop.setFromJsonObject(json::getObject(entryObj, "Stop"));

		if (newEntry.type == BerzierType) {
			newEntry.control1.setFromJsonObject(json::getObject(entryObj, "C1"));
			newEntry.control2.setFromJsonObject(json::getObject(entryObj, "C2"));
		}

		m_entries.push_back(newEntry);
	}
}

bool ot::Path2D::isClosed(void) {
	if (m_entries.empty()) return false;
	Point2D s = m_entries.front().start;
	Point2D c = s;
	for (const PathEntry& e : m_entries) {
		// Start Point = old end point
		if (e.start != c) return false;

		c = e.stop;
	}

	// Final stop = first point
	return c == s;
}

void ot::Path2D::lineTo(const Point2D& _dest) {
	if (m_pos == _dest) return;

	PathEntry newEntry;
	newEntry.type = LineType;
	newEntry.start = m_pos;
	newEntry.stop = _dest;

	m_pos = _dest;
	m_entries.push_back(newEntry);
}

void ot::Path2D::berzierTo(const Point2D& _ct1, const Point2D& _ct2, const Point2D& _dest) {
	if (m_pos == _dest) return;

	PathEntry newEntry;
	newEntry.type = BerzierType;
	newEntry.start = m_pos;
	newEntry.control1 = _ct1;
	newEntry.control2 = _ct2;
	newEntry.stop = _dest;

	m_pos = _dest;
	m_entries.push_back(newEntry);
}

void ot::Path2D::clear(void) {
	m_entries.clear();
}

std::string ot::Path2D::toString(PathEntryType _type) {
	switch (_type)
	{
	case ot::Path2D::LineType: return "Line";
	case ot::Path2D::BerzierType: return "Berzier";
	default:
		OT_LOG_E("Unknown path entry type (" + std::to_string(_type) + ")");
		return "Line";
	}
}

ot::Path2D::PathEntryType ot::Path2D::toPathEntryType(const std::string& _type) {
	if (_type == toString(LineType)) return LineType;
	else if (_type == toString(BerzierType)) return BerzierType;
	else {
		OT_LOG_E("Unknown path entry type \"" + _type + "\"");
		return LineType;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::Path2DF::Path2DF() {}

ot::Path2DF::Path2DF(const Point2DD& _startPos) : m_pos(_startPos) {}

ot::Path2DF::Path2DF(const Path2DF& _other)
	: m_pos(_other.m_pos), m_entries(_other.m_entries)
{}

ot::Path2DF::~Path2DF() {}

ot::Path2DF& ot::Path2DF::operator = (const Path2DF& _other) {
	m_pos = _other.m_pos;
	m_entries = _other.m_entries;

	return *this;
}

void ot::Path2DF::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	JsonObject posObj;
	m_pos.addToJsonObject(posObj, _allocator);
	_object.AddMember("Pos", posObj, _allocator);

	JsonArray entryArr;
	for (const PathEntry& e : m_entries) {
		JsonObject entryObj;
		entryObj.AddMember("Type", JsonString(toString(e.type), _allocator), _allocator);

		JsonObject startObj;
		e.start.addToJsonObject(startObj, _allocator);
		entryObj.AddMember("Start", startObj, _allocator);

		JsonObject stopObj;
		e.stop.addToJsonObject(stopObj, _allocator);
		entryObj.AddMember("Stop", stopObj, _allocator);

		if (e.type == BerzierType) {
			JsonObject c1Obj;
			e.control1.addToJsonObject(c1Obj, _allocator);
			entryObj.AddMember("C1", c1Obj, _allocator);

			JsonObject c2Obj;
			e.control2.addToJsonObject(c2Obj, _allocator);
			entryObj.AddMember("C2", c2Obj, _allocator);
		}

		entryArr.PushBack(entryObj, _allocator);
	}

	_object.AddMember("Entries", entryArr, _allocator);
}

void ot::Path2DF::setFromJsonObject(const ot::ConstJsonObject& _object) {
	this->clear();

	ConstJsonObject posObj = json::getObject(_object, "Pos");
	m_pos.setFromJsonObject(posObj);

	ConstJsonArray entryArr = json::getArray(_object, "Entries");
	for (JsonSizeType i = 0; i < entryArr.Size(); i++) {
		ConstJsonObject entryObj = json::getObject(entryArr, i);

		PathEntry newEntry;
		newEntry.type = toPathEntryType(json::getString(entryObj, "Type"));
		newEntry.start.setFromJsonObject(json::getObject(entryObj, "Start"));
		newEntry.stop.setFromJsonObject(json::getObject(entryObj, "Stop"));

		if (newEntry.type == BerzierType) {
			newEntry.control1.setFromJsonObject(json::getObject(entryObj, "C1"));
			newEntry.control2.setFromJsonObject(json::getObject(entryObj, "C2"));
		}

		m_entries.push_back(newEntry);
	}
}

bool ot::Path2DF::isClosed(void) {
	if (m_entries.empty()) return false;
	Point2DD s = m_entries.front().start;
	Point2DD c = s;
	for (const PathEntry& e : m_entries) {
		// Start Point = old end point
		if (e.start != c) return false;

		c = e.stop;
	}

	// Final stop = first point
	return c == s;
}

void ot::Path2DF::lineTo(const Point2DD& _dest) {
	if (m_pos == _dest) return;

	PathEntry newEntry;
	newEntry.type = LineType;
	newEntry.start = m_pos;
	newEntry.stop = _dest;

	m_pos = _dest;
	m_entries.push_back(newEntry);
}

void ot::Path2DF::berzierTo(const Point2DD& _ct1, const Point2DD& _ct2, const Point2DD& _dest) {
	if (m_pos == _dest) return;

	PathEntry newEntry;
	newEntry.type = BerzierType;
	newEntry.start = m_pos;
	newEntry.control1 = _ct1;
	newEntry.control2 = _ct2;
	newEntry.stop = _dest;

	m_pos = _dest;
	m_entries.push_back(newEntry);
}

void ot::Path2DF::clear(void) {
	m_entries.clear();
}

std::string ot::Path2DF::toString(PathEntryType _type) {
	switch (_type)
	{
	case ot::Path2D::LineType: return "Line";
	case ot::Path2D::BerzierType: return "Berzier";
	default:
		OT_LOG_E("Unknown path entry type (" + std::to_string(_type) + ")");
		return "Line";
	}
}

ot::Path2DF::PathEntryType ot::Path2DF::toPathEntryType(const std::string& _type) {
	if (_type == toString(LineType)) return LineType;
	else if (_type == toString(BerzierType)) return BerzierType;
	else {
		OT_LOG_E("Unknown path entry type \"" + _type + "\"");
		return LineType;
	}
}