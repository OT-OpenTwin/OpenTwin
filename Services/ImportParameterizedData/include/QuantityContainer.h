#pragma once
#include <stdint.h>
#include <set>
#include <list>
#include <string>
#include "OpenTwinCore/Variable.h"

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/array.hpp>

#include "VariableToBSONConverter.h"

class QuantityContainer
{
public:
	QuantityContainer(int32_t msmdIndex, std::set<std::string>& parameterAbbreviations, std::list<int32_t>& parameterValueIndices, int32_t quantityIndex, bool isFlatCollection);
	const bsoncxx::builder::basic::document* GetDocument() const { return &_mongoDocument; };
	
	void AddValue(ot::Variable& value);
	bool ParameterValueIndicesAndQuantityIndexAreMatching(const std::list<int32_t>& parameterValueIndices, int32_t quantityIndex);
	size_t GetValueArraySize() const { return _values.size(); };
	bsoncxx::builder::basic::document _mongoDocument;
	
private:
	VariableToBSONConverter _converter;
	std::list<ot::Variable> _values;
	bool _isFlatCollection;
	std::list<int32_t> _parameterValueIndices;
	int32_t _quantityIndex;
};