// @otlicense
// File: PropertyInputFactoryRegistrar.hpp
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
#include "OTWidgets/PropertyInputFactory.h"
#include "OTWidgets/PropertyInputFactoryRegistrar.h"

template <typename T>
ot::PropertyInputFactoryRegistrar<T>::PropertyInputFactoryRegistrar(const std::string& _key) {
	static_assert(std::is_constructible_v<T, QWidget*>,
		"Factory-registered type must be constructible with QWidget* as its only parameter.");

	PropertyInputFactory::registerPropertyInput(_key, [](QWidget* _parent) { return new T(_parent); });
}
