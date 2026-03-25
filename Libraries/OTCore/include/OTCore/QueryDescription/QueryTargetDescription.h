#pragma once
#include "OTCore/Tuple/TupleInstance.h"
#include "OTCore/Serializable.h"
#include "OTCore/CoreAPIExport.h"
namespace ot
{
	class OT_CORE_API_EXPORT QueryTargetDescription : public ot::Serializable
	{
	public:	
		QueryTargetDescription(const TupleInstance& _tupleInstance, const std::string& _mongoDBFieldName, const std::string& _targetLabel);
		QueryTargetDescription() = default;
		QueryTargetDescription(const QueryTargetDescription& _other) = default;
		QueryTargetDescription(QueryTargetDescription&& _other) = default;
		QueryTargetDescription& operator=(const QueryTargetDescription& _other) = default;
		QueryTargetDescription& operator=(QueryTargetDescription&& _other) = default;

		void setTupleInstance(const TupleInstance& _tupleInstance);
		void setMongoDBFieldName(const std::string& _mongoDBFieldName);
		void setTargetLabel(const std::string& _targetLabel);

		const std::string& getLabel() const ;
		const std::string& getMongoDBFieldName() const;
		const TupleInstance& getTupleInstance() const;

		void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		void setFromJsonObject(const ConstJsonObject& _jsonObject) override;

		void setDimension(std::vector<uint32_t> _dimensions) { m_dimensions = _dimensions; }
		const std::vector<uint32_t>& getDimensions() const  { return m_dimensions; }
	private:
		TupleInstance m_tupleDescription;
		std::string m_mongoDBFieldName;
		std::string m_targetLabel;
		std::vector<uint32_t> m_dimensions;
	};
}
