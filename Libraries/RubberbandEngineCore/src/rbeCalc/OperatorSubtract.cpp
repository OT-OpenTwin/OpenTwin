// @otlicense

/*
 *	File:		OperatorSubtract.cpp
 *	Package:	rbeCalc
 *
 *  Created on: September 07, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2021 Alexander Kuester
 *	This file is part of the RubberbandEngine package.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

// RBE header
#include <rbeCalc/OperatorSubtract.h>

using namespace rbeCalc;

OperatorSubtract::OperatorSubtract(AbstractCalculationItem * _lhv, AbstractCalculationItem * _rhv)
	: AbstractOperator(_lhv, _rhv) {}

coordinate_t OperatorSubtract::value(void) const {
	coordinate_t v = 0.;
	if (m_lhv) { v = m_lhv->value(); }
	if (m_rhv) { v -= m_rhv->value(); }
	return v;
}