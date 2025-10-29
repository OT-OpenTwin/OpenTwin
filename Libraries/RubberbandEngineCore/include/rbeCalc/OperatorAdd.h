// @otlicense

#pragma once

// RBE header
#include <rbeCalc/AbstractOperator.h>

namespace rbeCalc {

	class RBE_API_EXPORT OperatorAdd : public AbstractOperator {
	public:
		OperatorAdd(AbstractCalculationItem * _lhv, AbstractCalculationItem * _rhv);
		virtual ~OperatorAdd() {}

		virtual coordinate_t value(void) const override;
	};

}