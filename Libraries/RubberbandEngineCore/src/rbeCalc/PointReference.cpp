// @otlicense
 
 // RBE header
#include <rbeCalc/PointReference.h>
#include <rbeCore/Point.h>
#include <rbeCore/rbeAssert.h>

using namespace rbeCalc;

PointReference::PointReference(rbeCore::AbstractPoint * _point, rbeCore::eAxis _axis)
	: m_point(_point), m_axis(_axis) {
	
	rbeAssert(m_point, "nullptr provided @PointReference");
}

coordinate_t PointReference::value(void) const {
	switch (m_axis)
	{
	case rbeCore::U: return m_point->u();
	case rbeCore::V: return m_point->v();
	case rbeCore::W: return m_point->w();
	default:
		rbeAssert(0, "Unknown axis @PointReference");
		return 0.;
	}
}