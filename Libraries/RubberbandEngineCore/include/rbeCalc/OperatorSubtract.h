// @otlicense

#pragma once

// RBE header
#include <rbeCalc/AbstractOperator.h>

namespace rbeCalc {

	class RBE_API_EXPORT OperatorSubtract : public AbstractOperator {
	public:
		OperatorSubtract(AbstractCalculationItem * _lhv, AbstractCalculationItem * _rhv);
		virtual ~OperatorSubtract() {}

		virtual coordinate_t value(void) const override;
	};

}