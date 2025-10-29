// @otlicense

/*
 *	File:		Point.cpp
 *	Package:	rbeCore
 *
 *  Created on: September 07, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2021 Alexander Kuester
 *	This file is part of the RubberbandEngine package.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

// RBE header
#include <rbeCore/Point.h>
#include <rbeCalc/AbstractCalculationItem.h>

using namespace rbeCore;

Point::Point() : m_u(nullptr), m_v(nullptr), m_w(nullptr) {}

Point::Point(rbeCalc::AbstractCalculationItem * _u, rbeCalc::AbstractCalculationItem * _v, rbeCalc::AbstractCalculationItem * _w) : m_u(_u), m_v(_v), m_w(_w) {}

Point::~Point() {
	if (m_u) { delete m_u; }
	if (m_v) { delete m_v; }
	if (m_w) { delete m_w; }
}

// #################################################################################################

// Setter

void Point::replaceU(rbeCalc::AbstractCalculationItem * _u) {
	if (m_u) { delete m_u; }
	m_u = _u;
}

void Point::replaceV(rbeCalc::AbstractCalculationItem * _v) {
	if (m_v) { delete m_v; }
	m_v = _v;
}

void Point::replaceW(rbeCalc::AbstractCalculationItem * _w) {
	if (m_w) { delete m_w; }
	m_w = _w;
}

// #################################################################################################

// Getter

coordinate_t Point::u(void) const {
	if (m_u) { return m_u->value(); }
	return 0;
}

coordinate_t Point::v(void) const {
	if (m_v) { return m_v->value(); }
	return 0;
}

coordinate_t Point::w(void) const {
	if (m_w) { return m_w->value(); }
	return 0;
}
