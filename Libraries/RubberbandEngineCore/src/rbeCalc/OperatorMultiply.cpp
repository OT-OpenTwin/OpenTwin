// @otlicense

// RBE header
#include <rbeCalc/OperatorMultiply.h>

using namespace rbeCalc;

OperatorMultiply::OperatorMultiply(AbstractCalculationItem * _lhv, AbstractCalculationItem * _rhv)
	: AbstractOperator(_lhv, _rhv) {}

coordinate_t OperatorMultiply::value(void) const {
	coordinate_t v = 0.;
	if (m_lhv) { v = m_lhv->value(); }
	if (m_rhv) { v *= m_rhv->value(); }
	return v;
}