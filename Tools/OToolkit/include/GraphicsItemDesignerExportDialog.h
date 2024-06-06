//! @file GraphicsItemDesignerExportDialog.h
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OToolkit header
#include "GraphicsItemDesignerExportConfig.h"

// OpenTwin header
#include "OTWidgets/Dialog.h"

class GraphicsItemDesigner;

namespace ot {
	class CheckBox;
	class LineEdit;
	class FilePathEdit;
}

class GraphicsItemDesignerExportDialog : public ot::Dialog {
	Q_OBJECT
	OT_DECL_NODEFAULT(GraphicsItemDesignerExportDialog)
	OT_DECL_NOCOPY(GraphicsItemDesignerExportDialog)
public:
	GraphicsItemDesignerExportDialog(GraphicsItemDesigner* _designer);
	virtual ~GraphicsItemDesignerExportDialog();

	GraphicsItemDesignerExportConfig createExportConfig(void) const;

private Q_SLOTS:
	void slotExport(void);
	void slotCancel(void);
	void slotUpdateFilePath(void);

private:
	GraphicsItemDesigner* m_designer;

	ot::LineEdit* m_itemNameInput;
	ot::FilePathEdit* m_fileInput;
	ot::CheckBox* m_autoAlignInput;
	ot::CheckBox* m_moveableItemInput;
	ot::CheckBox* m_itemGridSnapInput;
};