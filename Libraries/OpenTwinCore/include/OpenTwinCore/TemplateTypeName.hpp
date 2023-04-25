#pragma once

#include "OpenTwinCore/TemplateTypeName.h"
#include "OpenTwinCore/otAssert.h"

namespace ot
{
	template <class T>
	static std::string TemplateTypeName<T>::getTypeName()
	{
		otAssert(0, "Not supported type");
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