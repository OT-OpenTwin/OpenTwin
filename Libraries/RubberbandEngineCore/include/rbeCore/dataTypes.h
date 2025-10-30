// @otlicense
// File: dataTypes.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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