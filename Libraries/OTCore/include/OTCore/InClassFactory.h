// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/Logging/Logger.h"

// std header
#include <map>
#include <functional>

#define OT_DECL_INCLASS_FACTORY(___className, ___resultObjectType, ___registrarName)                       \
public:                                                                                                    \
typedef std::function<___resultObjectType* ()> FactoryConstructorFunction;                                 \
friend class ___registrarName;                                                                             \
template <typename T> class ___registrarName {                                                             \
public: ___registrarName(const std::string& _objectClassName) {                                            \
		___className::registerClass(_objectClassName, []() -> ___resultObjectType* { return new T(); });    \
	}                                                                                                      \
};                                                                                                         \
protected:                                                                                                 \
		static void registerClass(const std::string& _className, FactoryConstructorFunction _constructor); \
		static std::map<std::string, FactoryConstructorFunction>& getRegisteredClasses();

#define OT_DECL_INCLASS_FACTORY_CPP(___className)                                                          \
void ___className::registerClass(const std::string& _className, FactoryConstructorFunction _constructor)   \
{                                                                                                          \
	if (getRegisteredClasses().find(_className) != getRegisteredClasses().end())                           \
	{                                                                                                      \
		OT_LOG_E("Class with name \"" + _className + "\" is already registered.");                         \
		return;                                                                                            \
	}                                                                                                      \
	else                                                                                                   \
	{                                                                                                      \
		getRegisteredClasses().emplace(_className, std::move(_constructor));                               \
	}                                                                                                      \
}                                                                                                          \
std::map<std::string, ___className::FactoryConstructorFunction>& ___className::getRegisteredClasses()      \
{                                                                                                          \
	static std::map<std::string, ___className::FactoryConstructorFunction> registeredClasses;              \
	return registeredClasses;                                                                              \
}
