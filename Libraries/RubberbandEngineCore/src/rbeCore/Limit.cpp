// @otlicense

// RBE header
#include <rbeCore/Limit.h>
#include <rbeCore/rbeAssert.h>
#include <rbeCalc/AbstractCalculationItem.h>

rbeCore::Limit::Limit(AxisLimit _axis, rbeCalc::AbstractCalculationItem * _value)
	: m_axis(_axis), m_value(_value)
{ rbeAssert(m_value, "The provided formula is NULL"); }

rbeCore::Limit::~Limit() { delete m_value; }

// ##########################################################################

// Getter

coordinate_t rbeCore::Limit::value(void) const { return m_value->value(); }
