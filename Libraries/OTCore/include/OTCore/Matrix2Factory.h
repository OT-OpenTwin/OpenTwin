// @otlicense
// File: Matrix2Factory.h
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
#include "OTCore/Matrix2.h"

// std header
#include <vector>

template <class T>
class Matrix2Factory {
public:
	static Matrix2<T>* createRowWiseOptimized(int64_t _rows, int64_t _columns, std::vector<std::vector<T>> _columnWiseData);
	static Matrix2<T>* createRowWiseOptimizedEmpty(int64_t _rows, int64_t _columns);
	static Matrix2<T>* createColumnWiseOptimized(int64_t _rows, int64_t _columns, std::vector<std::vector<T>> _columnWiseData);
	static Matrix2<T>* createColumnWiseOptimizedEmpty(int64_t _rows, int64_t _columns);
};

#include "OTCore/Matrix2Factory.hpp"