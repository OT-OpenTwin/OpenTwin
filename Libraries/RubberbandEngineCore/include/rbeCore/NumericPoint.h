/*
 *	File:		NumericPoint.h
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

// RBE header
#include <rbeCore/AbstractPoint.h>

namespace rbeCore {

	class RBE_API_EXPORT NumericPoint : public AbstractPoint {
	public:
		NumericPoint();
		NumericPoint(coordinate_t _u, coordinate_t _v, coordinate_t _w);
		virtual ~NumericPoint();

		// ################################################################################

		// Setter

		void set(coordinate_t _u, coordinate_t _v, coordinate_t _w);

		void setU(coordinate_t _u) { m_u = _u; }

		void setV(coordinate_t _v) { m_v = _v; }

		void setW(coordinate_t _w) { m_w = _w; }
		
		// ################################################################################

		// Getter

		virtual coordinate_t u(void) const override { return m_u; }

		virtual coordinate_t v(void) const override { return m_v; }

		virtual coordinate_t w(void) const override { return m_w; }

	private:
		coordinate_t		m_u;
		coordinate_t		m_v;
		coordinate_t		m_w;

		NumericPoint(NumericPoint&) = delete;
		NumericPoint& operator = (NumericPoint&) = delete;
	};

}