// @otlicense
// File: PointReference.cpp
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
 
 // RBE header
#include <rbeCalc/PointReference.h>
#include <rbeCore/Point.h>
#include <rbeCore/rbeAssert.h>

using namespace rbeCalc;

PointReference::PointReference(rbeCore::AbstractPoint * _point, rbeCore::eAxis _axis)
	: m_point(_point), m_axis(_axis) {
	
	rbeAssert(m_point, "nullptr provided @PointReference");
}

coordinate_t PointReference::value(void) const {
	switch (m_axis)
	{
	case rbeCore::U: return m_point->u();
	case rbeCore::V: return m_point->v();
	case rbeCore::W: return m_point->w();
	default:
		rbeAssert(0, "Unknown axis @PointReference");
		return 0.;
	}
}