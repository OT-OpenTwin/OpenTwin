// @otlicense

#pragma once

// RBE header
#include <rbeCalc/AbstractCalculationItem.h>

namespace rbeCore {
	class AbstractPoint;
}

namespace rbeCalc {

	class RBE_API_EXPORT PointReference : public AbstractCalculationItem {
	public:
		PointReference(rbeCore::AbstractPoint * _point, rbeCore::eAxis _axis);

		virtual coordinate_t value(void) const override;

	protected:

		rbeCore::AbstractPoint *	m_point;
		rbeCore::eAxis				m_axis;

	private:

		PointReference() = delete;
		PointReference(PointReference&) = delete;
		PointReference& operator = (PointReference&) = delete;
	};

}