// @otlicense

// RBE header
#include <rbeCalc/OperatorAdd.h>

using namespace rbeCalc;

OperatorAdd::OperatorAdd(AbstractCalculationItem * _lhv, AbstractCalculationItem * _rhv)
	: AbstractOperator(_lhv, _rhv) {}

coordinate_t OperatorAdd::value(void) const {
	coordinate_t v = 0.;
	if (m_lhv) { v = m_lhv->value(); }
	if (m_rhv) { v += m_rhv->value(); }
	return v;
}