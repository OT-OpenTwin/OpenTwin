// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/ValueProcessing/ValueProcessor.h"

namespace ot {

	class OT_CORE_API_EXPORT ValueProcessorLog : public ValueProcessor
	{
		OT_DECL_DEFCOPY(ValueProcessorLog)
		OT_DECL_DEFMOVE(ValueProcessorLog)
	public:
		enum class LogBase
		{
			Base10,
			Natural
		};

		ValueProcessorLog(double _multiplier = 1., LogBase _base = LogBase::Base10);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Processing

		ot::Variable execute(const ot::Variable& _input) override;
		OT_DECL_NODISCARD ValueProcessor* inverse() const override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Serialization

		OT_DECL_NODISCARD ValueProcessor* createCopy() const override { return new ValueProcessorLog(*this); };
		static std::string className() { return "ValueProcessorLog"; };
		std::string getClassName() const override { return ValueProcessorLog::className(); };

		virtual void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _jsonObject) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		inline void setMultiplier(double _multiplier) { m_multiplier = _multiplier; };
		inline double getMultiplier() const { return m_multiplier; };

		inline void setLogBase(LogBase _base) { m_base = _base; };
		inline LogBase getLogBase() const { return m_base; };

	private:
		double m_multiplier;
		LogBase m_base;

	};

}