//! @file Math.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Math.h"

// std header
#include <cmath>
#include <algorithm>

double ot::Math::euclideanDistance(double _x1, double _y1, double _x2, double _y2) {
    return std::sqrt((_x1 - _x2) * (_x1 - _x2) + (_y1 - _y2) * (_y1 - _y2));
}

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

ot::Point2DD ot::Math::calculatePointOnBezierCurve(double _t, double _startX, double _startY, double _control1X, double _control1Y, double _control2X, double _control2Y, double _endX, double _endY) {
    double u = 1 - _t;
    double tt = _t * _t;
    double uu = u * u;
    double uuu = uu * u;
    double ttt = tt * _t;

    return Point2DD(
        uuu * _startX + 3 * uu * _t * _control1X + 3 * u * tt * _control2X + ttt * _endX,
        uuu * _startY + 3 * uu * _t * _control1Y + 3 * u * tt * _control2Y + ttt * _endY
    );
}

double ot::Math::calculateShortestDistanceFromPointToBezierCurve(double _px, double _py, double _startX, double _startY, double _control1X, double _control1Y, double _control2X, double _control2Y, double _endX, double _endY) {
    double minDistance = std::numeric_limits<double>::max();

    const int segments = 100;

    // Iterate over discrete t values on the Bezier curve
    for (int i = 0; i <= segments; ++i) {
        double t = static_cast<double>(i) / static_cast<double>(segments);
        Point2DD segmentPoint = Math::calculatePointOnBezierCurve(t, _startX, _startY, _control1X, _control1Y, _control2X, _control2Y, _endX, _endY);
        double dist = Math::euclideanDistance(_px, _py, segmentPoint.x(), segmentPoint.y());
        if (dist < minDistance) {
            minDistance = dist;
        }
    }

    return minDistance;
}