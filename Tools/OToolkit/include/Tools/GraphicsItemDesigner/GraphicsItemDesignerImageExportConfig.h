// @otlicense
// File: GraphicsItemDesignerImageExportConfig.h
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

// Qt header
#include <QtCore/qstring.h>
#include <QtCore/qmargins.h>

class GraphicsItemDesignerImageExportConfig {
public:
	GraphicsItemDesignerImageExportConfig();
	GraphicsItemDesignerImageExportConfig(const GraphicsItemDesignerImageExportConfig& _other);
	virtual ~GraphicsItemDesignerImageExportConfig();

	GraphicsItemDesignerImageExportConfig& operator = (const GraphicsItemDesignerImageExportConfig& _other);

	void setFileName(const QString& _fileName) { m_fileName = _fileName; };
	const QString& getFileName(void) const { return m_fileName; };

	void setMargins(const QMarginsF& _margins) { m_margins = _margins; };
	const QMarginsF& getMargins(void) const { return m_margins; };

	void setShowGrid(bool _showGrid) { m_showGrid = _showGrid; };
	bool getShowGrid(void) const { return m_showGrid; };

private:
	QString m_fileName;
	QMarginsF m_margins;
	bool m_showGrid;
};