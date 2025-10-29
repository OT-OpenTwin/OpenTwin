// @otlicense

#pragma once

#include <rbeCalc/AbstractCalculationItem.h>

namespace rbeCalc {

	class RBE_API_EXPORT VariableValue : public AbstractCalculationItem {
	public:
		VariableValue() {}
		VariableValue(coordinate_t _v) : m_value(_v) {}
		virtual ~VariableValue() {}

		virtual coordinate_t value(void) const override { return m_value; }

		void setValue(coordinate_t _v) { m_value = _v; }

	protected:
		coordinate_t		m_value;
	};

}