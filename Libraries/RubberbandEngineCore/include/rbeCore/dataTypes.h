// @otlicense

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