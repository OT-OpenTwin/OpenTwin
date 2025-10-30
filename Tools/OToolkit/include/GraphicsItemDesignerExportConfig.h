// @otlicense
// File: GraphicsItemDesignerExportConfig.h
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
#include "OTSystem/Flags.h"

// Qt header
#include <QtCore/qstring.h>

class GraphicsItemDesignerExportConfig {
public:
	enum ExportConfigFlag {
		NoFlags = 0x00,
		AutoAlign = 0x01
	};
	typedef ot::Flags<ExportConfigFlag> ExportConfigFlags;

	GraphicsItemDesignerExportConfig();
	GraphicsItemDesignerExportConfig(const GraphicsItemDesignerExportConfig& _other);
	virtual ~GraphicsItemDesignerExportConfig();

	GraphicsItemDesignerExportConfig& operator = (const GraphicsItemDesignerExportConfig& _other);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

	void setExportConfigFlag(ExportConfigFlag _flag, bool _active = true) { m_exportConfigFlags.setFlag(_flag, _active); };
	void setExportConfigFlags(const ExportConfigFlags& _flags) { m_exportConfigFlags = _flags; };
	const ExportConfigFlags& getExportConfigFlags(void) const { return m_exportConfigFlags; };

	void setFileName(const QString& _fileName) { m_fileName = _fileName; };
	const QString& getFileName(void) const { return m_fileName; };

private:
	QString m_fileName;
	ExportConfigFlags m_exportConfigFlags;
};

OT_ADD_FLAG_FUNCTIONS(GraphicsItemDesignerExportConfig::ExportConfigFlag)