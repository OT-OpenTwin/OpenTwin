// @otlicense

// RBE header
#include <rbeCalc/AbstractOperator.h>

using namespace rbeCalc;

AbstractOperator::AbstractOperator(AbstractCalculationItem * _lhv, AbstractCalculationItem * _rhv) : m_lhv(_lhv), m_rhv(_rhv) {}

AbstractOperator::~AbstractOperator() {
	if (m_lhv) delete m_lhv;
	if (m_rhv) delete m_rhv;
}

void AbstractOperator::replaceLeft(AbstractCalculationItem * _lhv) {
	if (m_lhv) delete m_lhv;
	m_lhv = _lhv;
}

void AbstractOperator::replaceRight(AbstractCalculationItem * _rhv) {
	if (m_rhv) delete m_rhv;
	m_rhv = _rhv;
}