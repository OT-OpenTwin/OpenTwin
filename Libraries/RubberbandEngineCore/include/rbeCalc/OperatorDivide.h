// @otlicense

#pragma once

// RBE header
#include <rbeCalc/AbstractOperator.h>

namespace rbeCalc {

	class RBE_API_EXPORT OperatorDivide : public AbstractOperator {
	public:
		OperatorDivide(AbstractCalculationItem * _lhv, AbstractCalculationItem * _rhv);
		virtual ~OperatorDivide() {}

		virtual coordinate_t value(void) const override;
	};

}