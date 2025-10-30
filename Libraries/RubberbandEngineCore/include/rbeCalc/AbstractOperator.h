// @otlicense
// File: AbstractOperator.h
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
#include <rbeCalc/AbstractCalculationItem.h>

namespace rbeCalc {

	class RBE_API_EXPORT AbstractOperator : public AbstractCalculationItem {
	public:
		AbstractOperator(AbstractCalculationItem * _lhv = nullptr, AbstractCalculationItem * _rhv = nullptr);
		virtual ~AbstractOperator();

		AbstractCalculationItem * left(void) { return m_lhv; }

		AbstractCalculationItem * right(void) { return m_rhv; }

		void replaceLeft(AbstractCalculationItem * _lhv);

		void replaceRight(AbstractCalculationItem * _rhv);

	protected:
		AbstractCalculationItem *		m_lhv;
		AbstractCalculationItem *		m_rhv;

	private:

		AbstractOperator(AbstractOperator&) = delete;
		AbstractOperator& operator = (AbstractOperator&) = delete;
	};

}