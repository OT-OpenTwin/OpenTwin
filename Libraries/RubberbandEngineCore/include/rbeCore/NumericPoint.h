// @otlicense
// File: NumericPoint.h
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