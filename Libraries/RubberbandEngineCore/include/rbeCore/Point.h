// @otlicense
// File: Point.h
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

// Project header
#include <rbeCore/AbstractPoint.h>

namespace rbeCalc {
	class AbstractCalculationItem;
}

namespace rbeCore {

	class RBE_API_EXPORT Point : public AbstractPoint {
	public:

		Point();
		Point(rbeCalc::AbstractCalculationItem * _u, rbeCalc::AbstractCalculationItem * _v, rbeCalc::AbstractCalculationItem * _w);
		virtual ~Point();

		// #################################################################################################

		// Setter

		void replaceU(rbeCalc::AbstractCalculationItem * _u);

		void replaceV(rbeCalc::AbstractCalculationItem * _v);

		void replaceW(rbeCalc::AbstractCalculationItem * _w);

		// #################################################################################################

		// Getter
		
		virtual coordinate_t u(void) const override;

		virtual coordinate_t v(void) const override;

		virtual coordinate_t w(void) const override;

	private:
		rbeCalc::AbstractCalculationItem *		m_u;
		rbeCalc::AbstractCalculationItem *		m_v;
		rbeCalc::AbstractCalculationItem *		m_w;

		Point(Point&) = delete;
		Point& operator = (Point&) = delete;
	};

}