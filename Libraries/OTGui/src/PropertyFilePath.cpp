//! @file PropertyFilePath.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/Logger.h"
#include "OTGui/PropertyFilePath.h"
#include "OTGui/PropertyFactoryRegistrar.h"

static ot::PropertyFactoryRegistrar<ot::PropertyFilePath> propertyFilePathRegistrar(ot::PropertyFilePath::propertyTypeString());

ot::PropertyFilePath::PropertyFilePath(const PropertyFilePath* _other) 
	: Property(_other), m_browseMode(_other->m_browseMode), m_filters(_other->m_filters), m_path(_other->m_path)
{}

ot::PropertyFilePath::PropertyFilePath(const PropertyBase& _base)
	: Property(_base), m_browseMode(PropertyFilePath::ReadFile)
{}

ot::PropertyFilePath::PropertyFilePath(BrowseMode _mode, PropertyFlags _flags)
	: Property(_flags), m_browseMode(_mode)
{}

ot::PropertyFilePath::PropertyFilePath(const std::string& _path, BrowseMode _mode, PropertyFlags _flags)
	: Property(_flags), m_browseMode(_mode), m_path(_path)
{}

ot::PropertyFilePath::PropertyFilePath(const std::string& _name, const std::string& _path, BrowseMode _mode, PropertyFlags _flags)
	: Property(_name, _flags), m_browseMode(_mode), m_path(_path)
{}

void ot::PropertyFilePath::addFilter(const FilterInfo& _info) {
	m_filters.push_back(_info);
}

void ot::PropertyFilePath::mergeWith(const Property* _other, const MergeMode& _mergeMode) {
	Property::mergeWith(_other, _mergeMode);

	const PropertyFilePath* other = dynamic_cast<const PropertyFilePath*>(_other);
	OTAssertNullptr(other);

	if (_mergeMode & PropertyBase::MergeValues) {
		m_path = other->m_path;
	}
	if (_mergeMode & PropertyBase::MergeConfig) {
		m_browseMode = other->m_browseMode;
		m_filters = other->m_filters;
	}
}

ot::Property* ot::PropertyFilePath::createCopy(void) const {
	return new ot::PropertyFilePath(this);
}

void ot::PropertyFilePath::getPropertyData(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Path", JsonString(m_path, _allocator), _allocator);
	_object.AddMember("Mode", (bool)m_browseMode, _allocator);
	JsonArray fArr;
	for (const FilterInfo& info : m_filters) {
		JsonObject fObj;
		fObj.AddMember("Ext", JsonString(info.extension, _allocator), _allocator);
		fObj.AddMember("Txt", JsonString(info.text, _allocator), _allocator);
		fArr.PushBack(fObj, _allocator);
	}
	_object.AddMember("Filters", fArr, _allocator);
}

void ot::PropertyFilePath::setPropertyData(const ot::ConstJsonObject& _object) {
	m_path = json::getString(_object, "Path");
	m_browseMode = (BrowseMode)json::getBool(_object, "Mode");
	ConstJsonArray fArr = json::getArray(_object, "Filters");
	for (JsonSizeType i = 0; i < fArr.Size(); i++) {
		ConstJsonObject fObj = json::getObject(fArr, i);
		FilterInfo info;
		info.extension = json::getString(fObj, "Ext");
		info.text = json::getString(fObj, "Txt");
		m_filters.push_back(info);
	}
}