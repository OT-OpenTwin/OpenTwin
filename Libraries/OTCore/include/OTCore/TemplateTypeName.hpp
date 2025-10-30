// @otlicense
// File: TemplateTypeName.hpp
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

#include "OTCore/TemplateTypeName.h"
#include "OTCore/LogDispatcher.h"

namespace ot
{
	template <class T>
	static std::string TemplateTypeName<T>::getTypeName()
	{
		OT_LOG_EA("Not supported type");
	}

	template <>
	inline static std::string TemplateTypeName<int32_t>::getTypeName()
	{
		return ot::TypeNames::getInt32TypeName();
	}

	template <>
	inline static std::string TemplateTypeName<int64_t>::getTypeName()
	{
		return ot::TypeNames::getInt64TypeName();
	}

	template <>
	inline static std::string TemplateTypeName<std::string>::getTypeName()
	{
		return ot::TypeNames::getStringTypeName();
	}

	template <>
	inline static std::string TemplateTypeName<double>::getTypeName()
	{
		return ot::TypeNames::getDoubleTypeName();
	}

	template <>
	inline static std::string TemplateTypeName<float>::getTypeName()
	{
		return ot::TypeNames::getFloatTypeName();
	}

	template <>
	inline static std::string TemplateTypeName<char>::getTypeName()
	{
		return ot::TypeNames::getCharTypeName();
	}
}