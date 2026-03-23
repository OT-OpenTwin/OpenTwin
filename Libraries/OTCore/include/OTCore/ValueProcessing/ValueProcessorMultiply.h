// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/ValueProcessing/ValueProcessor.h"

namespace ot {

	class OT_CORE_API_EXPORT ValueProcessorMultiply : public ValueProcessor
	{
		OT_DECL_DEFCOPY(ValueProcessorMultiply)
		OT_DECL_DEFMOVE(ValueProcessorMultiply)
	public:
		ValueProcessorMultiply(double _factor = 1.);
		virtual ~ValueProcessorMultiply() = default;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Processing

		ot::Variable execute(const ot::Variable& _input) override;
		OT_DECL_NODISCARD ValueProcessor* inverse() const override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Serialization
		
		OT_DECL_NODISCARD ValueProcessor* createCopy() const override { return new ValueProcessorMultiply(*this); };
		static std::string className() { return "ValueProcessorMultiply"; };
		std::string getClassName() const override { return ValueProcessorMultiply::className(); };

		virtual void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _jsonObject) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		inline void setFactor(double _factor) { m_factor = _factor; };
		inline double getFactor() const { return m_factor; };

	private:
		double m_factor;

	};

}