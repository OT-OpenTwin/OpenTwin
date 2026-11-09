// @otlicense
// File: Point2D.hpp
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