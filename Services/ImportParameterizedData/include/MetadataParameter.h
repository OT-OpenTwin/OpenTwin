#pragma once
#include <string>
#include <list>
#include <map>
#include <stdint.h>

template <class T>
struct MetadataParameter
{
	std::string parameterName;
	std::string parameterAbbreviation;
	std::list<T> values;
	std::map<T, uint32_t> valueIndices;
};