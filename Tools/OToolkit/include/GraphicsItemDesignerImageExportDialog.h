// @otlicense
// File: GraphicsItemDesignerImageExportDialog.h
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
#include "GraphicsItemDesignerImageExportConfig.h"

// OpenTwin header
#include "OTWidgets/Dialog.h"

// Qt header
#include <QtGui/qimage.h>

class GraphicsItemDesigner;

namespace ot {
	class CheckBox;
	class ImagePreview;
	class DoubleSpinBox;
	class PathBrowseEdit;
}

class GraphicsItemDesignerImageExportDialog : public ot::Dialog {
	Q_OBJECT
	OT_DECL_NODEFAULT(GraphicsItemDesignerImageExportDialog)
	OT_DECL_NOCOPY(GraphicsItemDesignerImageExportDialog)
public:
	GraphicsItemDesignerImageExportDialog(GraphicsItemDesigner* _designer, QWidget* _parent);
	virtual ~GraphicsItemDesignerImageExportDialog();

	GraphicsItemDesignerImageExportConfig createImageExportConfig(void) const;

private Q_SLOTS:
	void slotExport(void);
	void slotUpdatePreview(void);

private:
	GraphicsItemDesigner* m_designer;

	ot::ImagePreview* m_imagePreview;

	ot::PathBrowseEdit* m_pathInput;
	ot::CheckBox* m_showGrid;
	ot::DoubleSpinBox* m_topMarginsInput;
	ot::DoubleSpinBox* m_leftMarginsInput;
	ot::DoubleSpinBox* m_rightMarginsInput;
	ot::DoubleSpinBox* m_bottomMarginsInput;
};