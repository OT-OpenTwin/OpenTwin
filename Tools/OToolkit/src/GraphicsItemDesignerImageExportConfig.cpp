// @otlicense
// File: GraphicsItemDesignerImageExportConfig.cpp
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

// OToolkit header
#include "GraphicsItemDesignerImageExportConfig.h"

GraphicsItemDesignerImageExportConfig::GraphicsItemDesignerImageExportConfig()
	: m_showGrid(false)
{}

GraphicsItemDesignerImageExportConfig::GraphicsItemDesignerImageExportConfig(const GraphicsItemDesignerImageExportConfig& _other) {
	*this = _other;
}

GraphicsItemDesignerImageExportConfig::~GraphicsItemDesignerImageExportConfig() {

}

GraphicsItemDesignerImageExportConfig& GraphicsItemDesignerImageExportConfig::operator = (const GraphicsItemDesignerImageExportConfig& _other) {
	if (this == &_other) return *this;

	m_fileName = _other.m_fileName;
	m_margins = _other.m_margins;
	m_showGrid = _other.m_showGrid;

	return *this;
}