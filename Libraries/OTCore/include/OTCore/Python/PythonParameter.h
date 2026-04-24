// @otlicense
#pragma once
#include "OTCore/Serializable.h"
#include <string>
#include "OTCore/CoreAPIExport.h"
class OT_CORE_API_EXPORT PythonParameter : public ot::Serializable
{

public:
	PythonParameter() = default;
	~PythonParameter() = default;
	PythonParameter(const PythonParameter& _other);
	PythonParameter(PythonParameter&& _other) noexcept;
	PythonParameter operator=(const PythonParameter& _other);
	PythonParameter operator=(PythonParameter&& _other) noexcept;
	void swap(PythonParameter& other);
	
	void setCallingEntity(const std::string& _entityName);
	void setAdditionalParameter(ot::JsonDocument&& _parameter);
	void setPythonParameterType(const std::string& _type);

	const std::string& getCallingEntity() const { return m_callingEntity; }
	const std::string& getPythonParameterType() const { return m_type; }
	const ot::JsonDocument& getAdditionalParameter() const { return m_parameter; }

	virtual void addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator) const override;
	virtual void setFromJsonObject(const ot::ConstJsonObject& _jsonObject) override;
private:
	std::string m_callingEntity;
	std::string m_type;
	ot::JsonDocument m_parameter;
};
