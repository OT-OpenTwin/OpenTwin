// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/ValueProcessing/ValueProcessor.h"

namespace ot {

	class OT_CORE_API_EXPORT ValueProcessorAdd : public ValueProcessor
	{
		OT_DECL_DEFCOPY(ValueProcessorAdd)
		OT_DECL_DEFMOVE(ValueProcessorAdd)
	public:
		ValueProcessorAdd(double _summand = 0.);
		virtual ~ValueProcessorAdd() = default;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Processing

		ot::Variable execute(const ot::Variable& _input) override;
		OT_DECL_NODISCARD ValueProcessor* inverse() const override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Serialization

		OT_DECL_NODISCARD ValueProcessor* createCopy() const override { return new ValueProcessorAdd(*this); };
		static std::string className() { return "ValueProcessorAdd"; };
		std::string getClassName() const override { return ValueProcessorAdd::className(); };

		virtual void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _jsonObject) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		inline void setSummand(double _summand) { m_summand = _summand; };
		inline double getSummand() const { return m_summand; };

	private:
		double m_summand;

	};

}