// @otlicense

/*
 *	File:		OperatorSubtract.h
 *	Package:	rbeCalc
 *
 *  Created on: September 07, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2021 Alexander Kuester
 *	This file is part of the RubberbandEngine package.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

// RBE header
#include <rbeCalc/AbstractOperator.h>

namespace rbeCalc {

	class RBE_API_EXPORT OperatorSubtract : public AbstractOperator {
	public:
		OperatorSubtract(AbstractCalculationItem * _lhv, AbstractCalculationItem * _rhv);
		virtual ~OperatorSubtract() {}

		virtual coordinate_t value(void) const override;
	};

}