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
	class PathBrowseEdit;
}

class GraphicsItemDesignerExportDialog : public ot::Dialog {
	Q_OBJECT
	OT_DECL_NODEFAULT(GraphicsItemDesignerExportDialog)
	OT_DECL_NOCOPY(GraphicsItemDesignerExportDialog)
public:
	GraphicsItemDesignerExportDialog(GraphicsItemDesigner* _designer);
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