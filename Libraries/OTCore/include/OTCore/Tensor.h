// @otlicense
// File: Tensor.h
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
#include "DataObject.h"
#include <memory>
#include <vector>

template <class T>
class Tensor : public DataObject
{
public:
	Tensor(std::vector<uint32_t>& dimensions, uint32_t memoryOptimizedDimension);
	Tensor(std::vector<uint32_t>&& dimensions, uint32_t memoryOptimizedDimension);

private:
	std::vector<uint32_t> _dimensions;
	T* _data = nullptr;

	enum Alignment { CacheLine64 = 64, AVX = 128, AVX2 = 256, AVX512 = 512 };
	int _defaultAlignment = Alignment::CacheLine64;
};
