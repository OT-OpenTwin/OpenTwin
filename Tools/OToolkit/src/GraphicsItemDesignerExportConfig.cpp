// @otlicense
// File: GraphicsItemDesignerExportConfig.cpp
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
#include "GraphicsItemDesignerExportConfig.h"

GraphicsItemDesignerExportConfig::GraphicsItemDesignerExportConfig() 
	: m_exportConfigFlags(AutoAlign)
{

}

GraphicsItemDesignerExportConfig::GraphicsItemDesignerExportConfig(const GraphicsItemDesignerExportConfig& _other) {
	*this = _other;
}

GraphicsItemDesignerExportConfig::~GraphicsItemDesignerExportConfig() {

}

GraphicsItemDesignerExportConfig& GraphicsItemDesignerExportConfig::operator = (const GraphicsItemDesignerExportConfig& _other) {
	if (this == &_other) return *this;
	
	m_fileName = _other.m_fileName;
	m_exportConfigFlags = _other.m_exportConfigFlags;

	return *this;
}
