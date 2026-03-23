// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"

// std header
#include <list>
#include <string>
#include <string>
#include <vector>

namespace ot {

	class OT_CORE_API_EXPORT TupleInstance : public ot::Serializable
	{
		OT_DECL_DEFCOPY(TupleInstance)
		OT_DECL_DEFMOVE(TupleInstance)
	public:
		explicit TupleInstance() = default;
		explicit TupleInstance(const std::string& _tupleTypeName);
		virtual ~TupleInstance() = default;

		bool operator==(const TupleInstance& _other) const;
		bool operator!=(const TupleInstance& _other) const;

		void addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator) const override;
		void setFromJsonObject(const ot::ConstJsonObject& _jsonObject) override;

		inline void setTupleTypeName(const std::string& _tupleTypeName);
		inline const std::string& getTupleTypeName() const { return m_tupleTypeName; }

		inline void setTupleFormatName(const std::string& _formatName);
		inline const std::string& getTupleFormatName() const { return m_tupleFormatName; }

		inline void setTupleUnits(const std::vector<std::string>& _units);
		inline const std::vector<std::string>& getTupleUnits() const { return m_tupleUnits; }

		inline void setTupleElementDataTypes(const std::vector<std::string>& _dataTypes);
		inline const std::vector<std::string>& getTupleElementDataTypes() const { return m_tupleElementDataTypes; }
		
		inline bool isSingle() const { return m_tupleTypeName.empty(); }

	private:
		std::string m_tupleTypeName;
		std::string m_tupleFormatName;
		std::vector<std::string> m_tupleUnits;
		std::vector<std::string> m_tupleElementDataTypes;
	};

}