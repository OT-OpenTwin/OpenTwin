#pragma once
#include <string>
#include <list>
#include <map>
#include <stdint.h>
#include <vector>

template <class T>
struct MetadataParameter
{
	std::string parameterName;
	std::string parameterAbbreviation;
	std::list<T> uniqueValues;
	std::vector<T> selectedValues;
	std::map<T, uint32_t> valueIndices;
};