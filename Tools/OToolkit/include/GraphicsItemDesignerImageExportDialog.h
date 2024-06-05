//! @file GraphicsItemDesignerImageExportDialog.h
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/Dialog.h"

class GraphicsItemDesigner;

class GraphicsItemDesignerImageExportDialog : public ot::Dialog {
	Q_OBJECT
	OT_DECL_NODEFAULT(GraphicsItemDesignerImageExportDialog)
	OT_DECL_NOCOPY(GraphicsItemDesignerImageExportDialog)
public:
	GraphicsItemDesignerImageExportDialog(GraphicsItemDesigner* _designer);
	virtual ~GraphicsItemDesignerImageExportDialog();

private Q_SLOTS:
	void slotExport(void);
	void slotCancel(void);

private:
	GraphicsItemDesigner* m_designer;

};