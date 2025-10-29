// @otlicense

// RBE header
#include <rbeCalc/OperatorDivide.h>

using namespace rbeCalc;

OperatorDivide::OperatorDivide(AbstractCalculationItem * _lhv, AbstractCalculationItem * _rhv)
	: AbstractOperator(_lhv, _rhv) {}

coordinate_t OperatorDivide::value(void) const {
	coordinate_t v = 0.;
	if (m_lhv) { v = m_lhv->value(); }
	if (m_rhv) {
		if (m_rhv->value() == 0) {
			return 0.;
		}
		v /= m_rhv->value();
	}
	else {
		return 0.;
	}
	return v;
}