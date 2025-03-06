//! @file GraphicsItemDesignerImageExportDialog.h
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

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
	class FilePathEdit;
	class ImagePreview;
	class DoubleSpinBox;
}

class GraphicsItemDesignerImageExportDialog : public ot::Dialog {
	Q_OBJECT
	OT_DECL_NODEFAULT(GraphicsItemDesignerImageExportDialog)
	OT_DECL_NOCOPY(GraphicsItemDesignerImageExportDialog)
public:
	GraphicsItemDesignerImageExportDialog(GraphicsItemDesigner* _designer);
	virtual ~GraphicsItemDesignerImageExportDialog();

	GraphicsItemDesignerImageExportConfig createImageExportConfig(void) const;

private Q_SLOTS:
	void slotExport(void);
	void slotUpdatePreview(void);

private:
	GraphicsItemDesigner* m_designer;

	ot::ImagePreview* m_imagePreview;

	ot::FilePathEdit* m_pathInput;
	ot::CheckBox* m_showGrid;
	ot::DoubleSpinBox* m_topMarginsInput;
	ot::DoubleSpinBox* m_leftMarginsInput;
	ot::DoubleSpinBox* m_rightMarginsInput;
	ot::DoubleSpinBox* m_bottomMarginsInput;
};