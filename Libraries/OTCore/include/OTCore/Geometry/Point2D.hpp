// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/Geometry/Point2D.h"

inline ot::Point2DF ot::Point2D::toPoint2DF() const
{
	return ot::Point2DF(static_cast<float>(m_x), static_cast<float>(m_y));
}

inline ot::Point2DD ot::Point2D::toPoint2DD() const
{
	return ot::Point2DD(static_cast<double>(m_x), static_cast<double>(m_y));
}
inline ot::Point2D ot::Point2DF::toPoint2D() const
{
	return ot::Point2D(static_cast<int>(m_x), static_cast<int>(m_y));
}

inline ot::Point2DD ot::Point2DF::toPoint2DD() const
{
	return ot::Point2DD(static_cast<double>(m_x), static_cast<double>(m_y));
}

inline ot::Point2D ot::Point2DD::toPoint2D() const
{
	return ot::Point2D(static_cast<int>(m_x), static_cast<int>(m_y));
}

inline ot::Point2DF ot::Point2DD::toPoint2DF() const
 {
	return ot::Point2DF(static_cast<float>(m_x), static_cast<float>(m_y));
}