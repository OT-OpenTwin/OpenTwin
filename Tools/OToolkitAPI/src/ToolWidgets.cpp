// @otlicense
// File: ToolWidgets.cpp
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

// Toolkit API header
#include "OToolkitAPI/ToolWidgets.h"

otoolkit::ToolWidgets::ToolWidgets()
	: m_toolBar(nullptr)
{}

otoolkit::ToolWidgets::ToolWidgets(const ToolWidgets& _other) 
{
	*this = _other;
}

otoolkit::ToolWidgets& otoolkit::ToolWidgets::operator = (const ToolWidgets& _other) {
	m_views = _other.m_views;
	m_toolBar = _other.m_toolBar;
	m_statusWidgets = _other.m_statusWidgets;

	return *this;
}
