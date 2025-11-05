// @otlicense
// File: GraphicsItemDesignerExportDialog.h
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

// OToolkit header
#include "GraphicsItemDesignerExportConfig.h"

// OpenTwin header
#include "OTWidgets/Dialog.h"

class GraphicsItemDesigner;

namespace ot {
	class CheckBox;
	class LineEdit;
	class PathBrowseEdit;
}

class GraphicsItemDesignerExportDialog : public ot::Dialog {
	Q_OBJECT
	OT_DECL_NODEFAULT(GraphicsItemDesignerExportDialog)
	OT_DECL_NOCOPY(GraphicsItemDesignerExportDialog)
public:
	GraphicsItemDesignerExportDialog(GraphicsItemDesigner* _designer, QWidget* _parent);
	virtual ~GraphicsItemDesignerExportDialog();

	GraphicsItemDesignerExportConfig createExportConfig(void) const;

protected:
	virtual void showEvent(QShowEvent* _event) override;

private Q_SLOTS:
	void slotExport(void);
	void slotCancel(void);
	
private:
	GraphicsItemDesigner* m_designer;

	ot::PathBrowseEdit* m_fileInput;
	ot::CheckBox* m_autoAlignInput;
};