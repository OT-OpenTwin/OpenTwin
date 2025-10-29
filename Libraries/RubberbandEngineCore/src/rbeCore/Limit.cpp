// @otlicense

/*
 *	File:		Limit.cpp
 *	Package:	rbeCore
 *
 *  Created on: November 12, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2021 Alexander Kuester
 *	This file is part of the RubberbandEngine package.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

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
