// @otlicense

// RBE header
#include <rbeCalc/OperatorSubtract.h>

using namespace rbeCalc;

OperatorSubtract::OperatorSubtract(AbstractCalculationItem * _lhv, AbstractCalculationItem * _rhv)
	: AbstractOperator(_lhv, _rhv) {}

coordinate_t OperatorSubtract::value(void) const {
	coordinate_t v = 0.;
	if (m_lhv) { v = m_lhv->value(); }
	if (m_rhv) { v -= m_rhv->value(); }
	return v;
}