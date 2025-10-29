// @otlicense

#pragma once

// RBE header
#include <rbeCore/dataTypes.h>

namespace rbeCalc { class AbstractCalculationItem; }

namespace rbeCore {

	//! A limit is used for the engine to forbid values exceeding a defined range of values
	//! Limits can be added to steps, so every step can have its own limits, also a step can have multiple limits
	class RBE_API_EXPORT Limit {
	public:
		Limit(AxisLimit _axis, rbeCalc::AbstractCalculationItem * _value);
		virtual ~Limit();

		// ##########################################################################

		// Getter

		AxisLimit axis(void) const { return m_axis; }

		coordinate_t value(void) const;

		rbeCalc::AbstractCalculationItem * valueItem(void) { return m_value; }

	private:

		AxisLimit								m_axis;
		rbeCalc::AbstractCalculationItem *		m_value;

		Limit() = delete;
		Limit(Limit&) = delete;
		Limit& operator = (Limit&) = delete;
	};

}
