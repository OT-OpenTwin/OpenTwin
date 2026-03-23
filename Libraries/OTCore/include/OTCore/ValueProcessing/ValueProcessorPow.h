// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/ValueProcessing/ValueProcessor.h"

namespace ot {

	class OT_CORE_API_EXPORT ValueProcessorPow : public ValueProcessor
	{
		OT_DECL_DEFCOPY(ValueProcessorPow)
		OT_DECL_DEFMOVE(ValueProcessorPow)
	public:
		ValueProcessorPow(double _multiplier = 1., double _exp = 1.);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Processing

		ot::Variable execute(const ot::Variable& _input) override;
		OT_DECL_NODISCARD ValueProcessor* inverse() const override;
		
		// ###########################################################################################################################################################################################################################################################################################################################

		// Serialization

		OT_DECL_NODISCARD ValueProcessor* createCopy() const override { return new ValueProcessorPow(*this); };
		static std::string className() { return "ValueProcessorPow"; };
		std::string getClassName() const override { return ValueProcessorPow::className(); };

		virtual void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _jsonObject) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		inline void setMultiplier(double _multiplier) { m_multiplier = _multiplier; };
		inline double getMultiplier() const { return m_multiplier; };

		inline void setExponent(double _exponent) { m_exponent = _exponent; };
		inline double getExponent() const { return m_exponent; };

	private:
		double m_multiplier;
		double m_exponent;

	};

}