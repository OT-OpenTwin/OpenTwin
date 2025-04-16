#pragma once
#include <string>
#include <list>
namespace ot
{
	struct __declspec(dllexport) ComparisionSymbols
	{
		inline static const std::string g_anyOneOfComparator = "any of";
		inline static const std::string g_noneOfComparator = "not any of";
		inline static const std::list<std::string> g_comparators = { "<", "<=", "=", ">", ">=", "!=", g_anyOneOfComparator, g_noneOfComparator };;
	};
}
