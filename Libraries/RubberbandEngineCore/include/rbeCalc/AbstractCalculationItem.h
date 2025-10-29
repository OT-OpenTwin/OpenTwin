// @otlicense

/*
 *	File:		AbstractCalculationItem.h
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
#include <rbeCore/dataTypes.h>

namespace rbeCalc {

	class RBE_API_EXPORT AbstractCalculationItem {
	public:
		AbstractCalculationItem() {}
		virtual ~AbstractCalculationItem() {}
		
		virtual coordinate_t value(void) const = 0;

	private:

		AbstractCalculationItem(AbstractCalculationItem&) = delete;
		AbstractCalculationItem& operator = (AbstractCalculationItem&) = delete;
	};

}