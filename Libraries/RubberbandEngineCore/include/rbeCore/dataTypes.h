/*
 *	File:		dataTypes.h
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

#ifdef RUBBERBANDENGINECORE_EXPORTS
#define RBE_API_EXPORT __declspec(dllexport)
#else
#define RBE_API_EXPORT __declspec(dllimport)
#endif // RUBBERBANDENGINECORE_EXPORTS

#if defined(RBE_COORDINATE_TYPE_INT)
typedef int coordinate_t;
#elif defined(RBE_COORDINATE_TYPE_DOUBLE)
typedef double coordinate_t;
#else
typedef float coordinate_t;
#endif // RBE_VALUE_TYPE_coordinate_t

namespace rbeCore {

	enum eAxis {
		U,
		V,
		W
	};

	enum eAxisDistance {
		dAll,
		dU,
		dV,
		dW,
		dUV,
		dUW,
		dVW,
		dVU,
		dWU,
		dWV
	};

	enum DefaultPoint {
		ORIGIN,
		CURRENT
	};

	enum CircleOrientation {
		coUV,
		coUW,
		coVW
	};

	enum AxisLimit {
		Umin,
		Umax,
		Vmin,
		Vmax,
		Wmin,
		Wmax
	};
}