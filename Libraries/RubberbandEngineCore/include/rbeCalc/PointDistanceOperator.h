/*
 *	File:		PointDistanceOperator.h
 *	Package:	rbeCalc
 *
 *  Created on: September 08, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2021 Alexander Kuester
 *	This file is part of the RubberbandEngine package.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

 // RBE header
#include <rbeCalc/AbstractCalculationItem.h>

namespace rbeCore {
	class AbstractPoint;
}

namespace rbeCalc {

	class RBE_API_EXPORT PointDistanceOperator : public AbstractCalculationItem {
	public:
		PointDistanceOperator(rbeCore::AbstractPoint * _lhv, rbeCore::AbstractPoint * _rhv, rbeCore::eAxisDistance _axis, bool _axisDistanceAppliesToBothPoints);
		virtual ~PointDistanceOperator() {}

		virtual coordinate_t value(void) const override;

	private:
		rbeCore::AbstractPoint *	m_lhv;
		rbeCore::AbstractPoint *	m_rhv;
		rbeCore::eAxisDistance		m_axis;
		bool						m_axisDistanceAppliesToBothPoints;
	};

}