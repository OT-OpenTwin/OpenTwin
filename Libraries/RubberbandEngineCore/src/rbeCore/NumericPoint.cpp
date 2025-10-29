// @otlicense

// RBE header
#include <rbeCore/NumericPoint.h>

using namespace rbeCore;

NumericPoint::NumericPoint() : m_u(0), m_v(0), m_w(0) {}

NumericPoint::NumericPoint(coordinate_t _u, coordinate_t _v, coordinate_t _w)
	: m_u(_u), m_v(_v), m_w(_w) {}

NumericPoint::~NumericPoint() {}

// ################################################################################

// Setter

void NumericPoint::set(coordinate_t _u, coordinate_t _v, coordinate_t _w) {
	m_u = _u;
	m_v = _v;
	m_w = _w;
}