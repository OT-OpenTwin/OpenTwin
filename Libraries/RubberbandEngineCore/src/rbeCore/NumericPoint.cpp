/*
 *	File:		NumericPoint.cpp
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