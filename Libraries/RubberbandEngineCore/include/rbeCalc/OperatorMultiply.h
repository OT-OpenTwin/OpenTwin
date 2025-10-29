// @otlicense

#pragma once

// RBE header
#include <rbeCalc/AbstractOperator.h>

namespace rbeCalc {

	class RBE_API_EXPORT OperatorMultiply : public AbstractOperator {
	public:
		OperatorMultiply(AbstractCalculationItem * _lhv, AbstractCalculationItem * _rhv);
		virtual ~OperatorMultiply() {}

		virtual coordinate_t value(void) const override;
	};

}