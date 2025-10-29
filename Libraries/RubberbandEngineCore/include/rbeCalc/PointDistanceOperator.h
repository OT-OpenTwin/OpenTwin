// @otlicense

#pragma once

 // RBE header
#include <rbeCalc/AbstractCalculationItem.h>

namespace rbeCore {
	class AbstractPoint;
}

namespace rbeCalc {

	class RBE_API_EXPORT PointDistanceOperator : public AbstractCalculationItem {
	public:
		PointDistanceOperator(rbeCore::AbstractPoint * _lhv, rbeCore::AbstractPoint * _rhv, rbeCore::eAxisDistance _axis, bool _axisDistanceAppliesToBothPoints);
		virtual ~PointDistanceOperator() {}

		virtual coordinate_t value(void) const override;

	private:
		rbeCore::AbstractPoint *	m_lhv;
		rbeCore::AbstractPoint *	m_rhv;
		rbeCore::eAxisDistance		m_axis;
		bool						m_axisDistanceAppliesToBothPoints;
	};

}