#pragma once
#include <stdint.h>
#include <list>
#include <string>
#include <set>

#include "OpenTwinCore/Variable.h"


#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/array.hpp>


class __declspec(dllexport) QuantityContainer
{
public:
	QuantityContainer(int32_t msmdIndex, std::set<std::string>& parameterAbbreviations, std::list<int32_t>& parameterValueIndices, int32_t quantityIndex, bool isFlatCollection);
	const bsoncxx::builder::basic::document* GetDocument() const { return &_mongoDocument; };

	void AddValue(ot::Variable& value);
	int64_t GetValueArraySize() const { return _values.size(); };
	const bsoncxx::builder::basic::document& getMongoDocument();

private:

	bsoncxx::builder::basic::document _mongoDocument;
	
	std::list<ot::Variable> _values;
	bool _isFlatCollection;
};
