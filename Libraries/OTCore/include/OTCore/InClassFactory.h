// @otlicense
// File: InClassFactory.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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
