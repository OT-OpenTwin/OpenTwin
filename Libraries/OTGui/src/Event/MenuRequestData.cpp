// @otlicense

// OpenTwin header
#include "OTCore/Logging/LogDispatcher.h"
#include "OTGui/Event/MenuRequestData.h"

namespace ot {
	void MenuRequestData::registerClass(const std::string& _className, FactoryConstructorFunction _constructor)
	{
		if (getRegisteredClasses().find(_className) != getRegisteredClasses().end())
		{
			ot::LogDispatcher::instance().dispatch("Class with name \"" + _className + "\" is already registered.", __FUNCTION__, ot::ERROR_LOG); return;
		}
		else
		{
			getRegisteredClasses().emplace(_className, std::move(_constructor));
		}
	} std::map<std::string, MenuRequestData::FactoryConstructorFunction>& MenuRequestData::getRegisteredClasses()
	{
		static std::map<std::string, MenuRequestData::FactoryConstructorFunction> registeredClasses; return registeredClasses;
	}
}

ot::MenuRequestData* ot::MenuRequestData::fromJson(const std::string& _jsonString)
{
	JsonDocument document;
	if (!document.fromJson(_jsonString))
	{
		OT_LOG_E("Failed to parse JSON string for MenuRequestData.");
		return nullptr;
	}
	return fromJson(document.getConstObject());
}

ot::MenuRequestData* ot::MenuRequestData::fromJson(const ConstJsonObject& _jsonObject)
{
	std::string className = json::getString(_jsonObject, "ClassName");
	auto it = getRegisteredClasses().find(className);
	if (it != getRegisteredClasses().end())
	{
		auto constructor = it->second;
		MenuRequestData* instance = constructor();
		instance->setFromJsonObject(_jsonObject);
		return instance;
	}
	else
	{
		OT_LOG_E("No registered MenuRequestData class found for class name \"" + className + "\".");
		return nullptr;
	}
}

ot::MenuRequestData::MenuRequestData(WidgetViewBase::ViewType _viewType)
	: m_viewType(_viewType)
{}

ot::MenuRequestData::MenuRequestData(const BasicEntityInformation & _basicEntityInformation, WidgetViewBase::ViewType _viewType)
	: BasicEntityInformation(_basicEntityInformation), m_viewType(_viewType)
{}

ot::MenuRequestData::MenuRequestData(BasicEntityInformation&& _basicEntityInformation, WidgetViewBase::ViewType _viewType) noexcept
	: BasicEntityInformation(std::move(_basicEntityInformation)), m_viewType(_viewType)
{}

void ot::MenuRequestData::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const
{
	BasicEntityInformation::addToJsonObject(_object, _allocator);
	static constexpr const char* classNameJsonKey = "ClassName";
	static int32_t jsonKeyLength = static_cast<int32_t>(std::strlen("ClassName"));
	_object.AddMember(JsonStringRef(classNameJsonKey, jsonKeyLength), JsonString(this->getClassName(), _allocator), _allocator);

	_object.AddMember("ViewType", static_cast<uint64_t>(m_viewType), _allocator);
}

void ot::MenuRequestData::setFromJsonObject(const ConstJsonObject& _object)
{
	BasicEntityInformation::setFromJsonObject(_object);
	m_viewType = static_cast<WidgetViewBase::ViewType>(json::getUInt64(_object, "ViewType"));
}

