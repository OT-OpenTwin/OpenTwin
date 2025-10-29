// @otlicense

/*
 *	File:		Point.h
 *	Package:	rbeCore
 *
 *  Created on: September 07, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2021 Alexander Kuester
 *	This file is part of the RubberbandEngine package.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

// Project header
#include <rbeCore/AbstractPoint.h>

namespace rbeCalc {
	class AbstractCalculationItem;
}

namespace rbeCore {

	class RBE_API_EXPORT Point : public AbstractPoint {
	public:

		Point();
		Point(rbeCalc::AbstractCalculationItem * _u, rbeCalc::AbstractCalculationItem * _v, rbeCalc::AbstractCalculationItem * _w);
		virtual ~Point();

		// #################################################################################################

		// Setter

		void replaceU(rbeCalc::AbstractCalculationItem * _u);

		void replaceV(rbeCalc::AbstractCalculationItem * _v);

		void replaceW(rbeCalc::AbstractCalculationItem * _w);

		// #################################################################################################

		// Getter
		
		virtual coordinate_t u(void) const override;

		virtual coordinate_t v(void) const override;

		virtual coordinate_t w(void) const override;

	private:
		rbeCalc::AbstractCalculationItem *		m_u;
		rbeCalc::AbstractCalculationItem *		m_v;
		rbeCalc::AbstractCalculationItem *		m_w;

		Point(Point&) = delete;
		Point& operator = (Point&) = delete;
	};

}