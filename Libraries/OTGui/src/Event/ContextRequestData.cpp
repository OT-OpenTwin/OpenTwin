// @otlicense

// OpenTwin header
#include "OTCore/Logging/LogDispatcher.h"
#include "OTGui/Event/ContextRequestData.h"

static ot::ContextRequestData::Registrar<ot::ContextRequestData> registrar(ot::ContextRequestData::className());

ot::ContextRequestData* ot::ContextRequestData::fromJson(const std::string& _jsonString)
{
	JsonDocument document;
	if (!document.fromJson(_jsonString))
	{
		OT_LOG_E("Failed to parse JSON string for ContextRequestData.");
		return nullptr;
	}
	return fromJson(document.getConstObject());
}

ot::ContextRequestData* ot::ContextRequestData::fromJson(const ConstJsonObject& _jsonObject)
{
	std::string className = json::getString(_jsonObject, ContextRequestData::classNameJsonKey());
	auto it = getRegisteredClasses().find(className);
	if (it != getRegisteredClasses().end())
	{
		ContextConstructorFunction constructor = it->second;
		ContextRequestData* instance = constructor();
		instance->setFromJsonObject(_jsonObject);
		return instance;
	}
	else
	{
		OT_LOG_E("No registered ContextRequestData class found for class name \"" + className + "\".");
		return nullptr;
	}
}

void ot::ContextRequestData::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const
{
	BasicEntityInformation::addToJsonObject(_object, _allocator);
	static int32_t jsonKeyLength = static_cast<int32_t>(std::strlen(classNameJsonKey()));
	_object.AddMember(JsonStringRef(classNameJsonKey(), jsonKeyLength), JsonString(getClassName(), _allocator), _allocator);

	_object.AddMember("Position", JsonObject(m_pos, _allocator), _allocator);
	_object.AddMember("ViewType", static_cast<uint64_t>(m_viewType), _allocator);
}

void ot::ContextRequestData::setFromJsonObject(const ConstJsonObject& _object)
{
	BasicEntityInformation::setFromJsonObject(_object);
	m_pos.setFromJsonObject(json::getObject(_object, "Position"));
	m_viewType = static_cast<WidgetViewBase::ViewType>(json::getUInt64(_object, "ViewType"));
}

void ot::ContextRequestData::registerClass(const std::string& _className, ContextConstructorFunction _constructor)
{
	if (getRegisteredClasses().find(_className) != getRegisteredClasses().end())
	{
		OT_LOG_E("ContextRequestData class with name \"" + _className + "\" is already registered.");
		return;
	}
	else
	{
		getRegisteredClasses().emplace(_className, std::move(_constructor));
	}
}

std::map<std::string, ot::ContextRequestData::ContextConstructorFunction>& ot::ContextRequestData::getRegisteredClasses()
{
	static std::map<std::string, ot::ContextRequestData::ContextConstructorFunction> registeredClasses;
	return registeredClasses;
}
