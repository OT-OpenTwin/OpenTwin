#pragma once
#include <stdint.h>
#include <list>
#include <string>
#include <map>

#include "OTCore/Variable.h"
#include "OTCore/CoreTypes.h"

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/array.hpp>


class __declspec(dllexport) QuantityContainer
{
public:
	QuantityContainer(int64_t _seriesIndex, std::list<ot::UID>& _parameterIDs, std::list<ot::Variable>&& _parameterValues, int64_t _quantityIndex);
	QuantityContainer(int64_t _seriesIndex, std::list<ot::UID>& _parameterIDs, std::list<ot::Variable>& _parameterValues, int64_t _quantityIndex);
	QuantityContainer(QuantityContainer&& _other) noexcept;
	QuantityContainer(const QuantityContainer& _other) = delete;
	QuantityContainer& operator=(const QuantityContainer& _other) = delete;
	QuantityContainer& operator=(QuantityContainer&& _other);
	~QuantityContainer();

	void addValue(const ot::Variable& _value);
	int64_t getValueArraySize() const { return m_values.size(); };
	bsoncxx::builder::basic::document& getMongoDocument();
	static const std::string getFieldName() { return "Values"; }

private:
	bsoncxx::builder::basic::document m_mongoDocument;
	std::list<ot::Variable> m_values;
};
