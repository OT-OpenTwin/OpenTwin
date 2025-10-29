// @otlicense

/*
 *	File:		PointDistanceOperator.cpp
 *	Package:	rbeCalc
 *
 *  Created on: September 08, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2021 Alexander Kuester
 *	This file is part of the RubberbandEngine package.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

// RBE header
#include <rbeCalc/PointDistanceOperator.h>
#include <rbeCore/AbstractPoint.h>
#include <rbeCore/rbeAssert.h>

// C++ header

using namespace rbeCalc;

PointDistanceOperator::PointDistanceOperator(rbeCore::AbstractPoint * _lhv, rbeCore::AbstractPoint * _rhv, rbeCore::eAxisDistance _axis, bool _axisDistanceAppliesToBothPoints)
	: m_lhv(_lhv), m_rhv(_rhv), m_axis(_axis), m_axisDistanceAppliesToBothPoints(_axisDistanceAppliesToBothPoints) {

	rbeAssert(m_lhv, "nullptr provided @PointDistanceOperator");
	rbeAssert(m_rhv, "nullptr provided @PointDistanceOperator");
}

coordinate_t PointDistanceOperator::value(void) const {
	if (m_axisDistanceAppliesToBothPoints) {
		return m_lhv->distance(*m_rhv, m_axis);
	}
	else {
		coordinate_t d;
		switch (m_axis)
		{
		case rbeCore::dAll: return m_lhv->distance(*m_rhv, m_axis);
		case rbeCore::dU: return m_lhv->distance(*m_rhv, m_axis);
		case rbeCore::dV: return m_lhv->distance(*m_rhv, m_axis);
		case rbeCore::dW: return m_lhv->distance(*m_rhv, m_axis);
		case rbeCore::dUV: d = m_lhv->u() - m_rhv->v(); break;
		case rbeCore::dUW: d = m_lhv->u() - m_rhv->w(); break;
		case rbeCore::dVW: d = m_lhv->v() - m_rhv->w(); break;
		case rbeCore::dVU: d = m_lhv->v() - m_rhv->u(); break;
		case rbeCore::dWU: d = m_lhv->w() - m_rhv->u(); break;
		case rbeCore::dWV: d = m_lhv->w() - m_rhv->v(); break;
		default:
			rbeAssert(0, "Invalid axis distance value @PointDistanceOperator");
			break;
		}

		if (d < 0) { d *= (-1); }
		return d;
	}
}