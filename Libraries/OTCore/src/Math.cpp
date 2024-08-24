//! @file Math.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Math.h"

// std header
#include <cmath>
#include <algorithm>

double ot::Math::calculateShortestDistanceFromPointToLine(double _px, double _py, double _x1, double _y1, double _x2, double _y2) {
    // Calculate vector for line (X2 - X1, Y2 - Y1)
    double dx = _x2 - _x1;
    double dy = _y2 - _y1;

    // Calculate projection t
    double t = ((_px - _x1) * dx + (_py - _y1) * dy) / (dx * dx + dy * dy);

    // Limit projection t
    t = std::max(0.0, std::min(1.0, t));

    // Calculate projection point P' on line
    double closestX = _x1 + t * dx;
    double closestY = _y1 + t * dy;

    // Calculate distance from P to P'
    double distance = std::sqrt((_px - closestX) * (_px - closestX) + (_py - closestY) * (_py - closestY));

    return distance;
}