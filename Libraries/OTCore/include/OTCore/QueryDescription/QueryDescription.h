#pragma once
#include "OTCore/QueryDescription/ValueComparisonDescription.h"
#include "OTCore/Tuple/TupleInstance.h"
#include "OTCore/QueryDescription/QueryTargetDescription.h"
#include <string>
#include "OTCore/Serializable.h"
namespace ot
{
	class OT_CORE_API_EXPORT QueryDescription : public Serializable
	{
	public:
		QueryDescription(const ot::ValueComparisonDescription& _comparisonDescription, const QueryTargetDescription& _queryComparison);
		QueryDescription() = default;
		QueryDescription(const QueryDescription& _other) = default;
		QueryDescription(QueryDescription&& _other) = default;
		QueryDescription& operator=(const QueryDescription& _other) = default;
		QueryDescription& operator=(QueryDescription&& _other) = default;

		void setQueryTargetDescription(const QueryTargetDescription& _queryTargetDescription);
		void setComparisonDescription(const ot::ValueComparisonDescription& _valueComparison);

		const QueryTargetDescription& getQueryTargetDescription();
		const ot::ValueComparisonDescription& getValueComparisonDescription();

		void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		void setFromJsonObject(const ConstJsonObject& _jsonObject) override;
	private:
		QueryTargetDescription m_queryTargetDescription;
		ot::ValueComparisonDescription m_comparisonDescription;
	};
}
