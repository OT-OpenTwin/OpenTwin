//! @file PropertyFilePath.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/Logger.h"
#include "OTCore/PropertyFilePath.h"
#include "OTCore/PropertyFactoryRegistrar.h"

static ot::PropertyFactoryRegistrar<ot::PropertyFilePath> propertyFilePathRegistrar(OT_PROPERTY_TYPE_FilePath);

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

ot::Property* ot::PropertyFilePath::createCopy(void) const {
	ot::PropertyFilePath* newProp = new ot::PropertyFilePath;
	newProp->setFromOther(this);

	newProp->setPath(this->path());
	newProp->setBrowseMode(this->browseMode());
	newProp->setFilters(this->filters());

	return newProp;
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