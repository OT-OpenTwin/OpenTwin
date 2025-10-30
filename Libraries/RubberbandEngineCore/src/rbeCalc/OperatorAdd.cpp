// @otlicense
// File: OperatorAdd.cpp
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
#include <rbeCalc/OperatorAdd.h>

using namespace rbeCalc;

OperatorAdd::OperatorAdd(AbstractCalculationItem * _lhv, AbstractCalculationItem * _rhv)
	: AbstractOperator(_lhv, _rhv) {}

coordinate_t OperatorAdd::value(void) const {
	coordinate_t v = 0.;
	if (m_lhv) { v = m_lhv->value(); }
	if (m_rhv) { v += m_rhv->value(); }
	return v;
}