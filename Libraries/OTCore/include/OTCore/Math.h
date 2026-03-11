// @otlicense
// File: Math.h
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

// OpenTwin header
#include "OTCore/Point2D.h"

// std header
#include <cmath>
#include <algorithm>

namespace ot {

	//! @brief The Math class provides mathematical constants and general utility functions.
	class OT_CORE_API_EXPORT Math {
	public:

		enum ComplexRepresentation {
			RealImaginary, //! Complex number represented as real and imaginary parts
			MagnitudePhase //! Complex number represented as magnitude and phase
		};
		static std::string toString(ComplexRepresentation _representation);
		static ComplexRepresentation stringToComplexRepresentation(const std::string& _representation);

		// ###########################################################################################################################################################################################################################################################################################################################

		// pi

		static constexpr double pi() noexcept { return 3.141592653589793238462643383279502884; };
		static constexpr double e() noexcept { return 2.718281828459045235360287471352662498; }

		// ###########################################################################################################################################################################################################################################################################################################################
		
		// Conversion

		static constexpr double degToRad(double _value) noexcept { return _value * pi() / 180.0; };
		static constexpr double radToDeg(double _value) noexcept { return _value * 180.0 / pi(); };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Scaling with decibels

		static inline double scaleWithDB10(double _value) noexcept { return 10 * std::log10(_value); };
		static inline double invScaleWithDB10(double _value) noexcept { return std::pow(10, _value / 10); };
		static inline double scaleWithDB20(double _value) noexcept { return 20 * std::log10(_value); };
		static inline double invScaleWithDB20(double _value) noexcept { return std::pow(10, _value / 20); };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Distance calculations

		//! @brief Calculates the Euclidean distance betwenn the two given points.
		//! @param _x1 Point 1 X.
		//! @param _y1 Point 1 Y.
		//! @param _x2 Point 2 X.
		//! @param _y2 Point 2 Y.
		static double euclideanDistance(double _x1, double _y1, double _x2, double _y2);

		//! @see double euclideanDistance(double _x1, double _y1, double _x2, double _y2)
		static double euclideanDistance(const Point2DD& _p1, const Point2DD& _p2) { return Math::euclideanDistance(_p1.x(), _p1.y(), _p2.x(), _p2.y()); };

		//! @brief Calculates the shortest distance from the given point to the given line.
		//! @param _px Point X.
		//! @param _py Point Y.
		//! @param _x1 Line Start X.
		//! @param _y1 Line Start Y.
		//! @param _x2 Line End X.
		//! @param _y2 Line End Y.
		static double calculateShortestDistanceFromPointToLine(double _px, double _py, double _x1, double _y1, double _x2, double _y2);

		//! @see double calculateShortestDistanceFromPointToLine(double _px, double _py, double _x1, double _y1, double _x2, double _y2)
		static double calculateShortestDistanceFromPointToLine(const Point2DD& _pt, const Point2DD& _lineStart, const Point2DD& _lineEnd) { return Math::calculateShortestDistanceFromPointToLine(_pt.x(), _pt.y(), _lineStart.x(), _lineStart.y(), _lineEnd.x(), _lineEnd.y()); };

		//! @brief Calculates the point on the Bezier curve for a given t value.
		//! @param _t The parameter value, ranging from 0 to 1, where 0 corresponds to the start of the curve and 1 corresponds to the end.
		//! @param _startX Bezier curve start point X.
		//! @param _startY Bezier curve start point Y.
		//! @param _control1X Bezier curve control point 1 X.
		//! @param _control1Y Bezier curve control point 1 Y.
		//! @param _control2X Bezier curve control point 2 X.
		//! @param _control2Y Bezier curve control point 2 Y.
		//! @param _endX Bezier curve end point X.
		//! @param _endY Bezier curve end point Y.
		static Point2DD calculatePointOnBezierCurve(double _t, double _startX, double _startY, double _control1X, double _control1Y, double _control2X, double _control2Y, double _endX, double _endY);

		//! @see Point2DD calculatePointOnBezierCurve(double _t, double _startX, double _startY, double _control1X, double _control1Y, double _control2X, double _control2Y, double _endX, double _endY)
		static Point2DD calculatePointOnBezierCurve(double _t, const Point2DD& _start, const Point2DD& _control1, const Point2DD& _control2, const Point2DD& _end) { return Math::calculatePointOnBezierCurve(_t, _start.x(), _start.y(), _control1.x(), _control1.y(), _control2.x(), _control2.y(), _end.x(), _end.y()); };

		//! @brief Calculates the minimum distance from a point to a cubic Bezier curve.
		//! @param _px Point X.
		//! @param _py Point Y.
		//! @param _startX Bezier curve start point X.
		//! @param _startY Bezier curve start point Y.
		//! @param _control1X Bezier curve control point 1 X.
		//! @param _control1Y Bezier curve control point 1 Y.
		//! @param _control2X Bezier curve control point 2 X.
		//! @param _control2Y Bezier curve control point 2 Y.
		//! @param _endX Bezier curve end point X.
		//! @param _endY Bezier curve end point Y.
		static double calculateShortestDistanceFromPointToBezierCurve(double _px, double _py, double _startX, double _startY, double _control1X, double _control1Y, double _control2X, double _control2Y, double _endX, double _endY);

		//! @see Point2DD calculateShortestDistanceFromPointToBezierCurve(double _px, double _py, double _startX, double _startY, double _control1X, double _control1Y, double _control2X, double _control2Y, double _endX, double _endY)
		static double calculateShortestDistanceFromPointToBezierCurve(const Point2DD& _pt, const Point2DD& _start, const Point2DD& _control1, const Point2DD& _control2, const Point2DD& _end) { return Math::calculateShortestDistanceFromPointToBezierCurve(_pt.x(), _pt.y(), _start.x(), _start.y(), _control1.x(), _control1.y(), _control2.x(), _control2.y(), _end.x(), _end.y()); };

	};

}