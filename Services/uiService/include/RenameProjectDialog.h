//! @file RenameProjectDialog.h
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/Dialog.h"

namespace ot { class LineEdit; };

class RenameProjectDialog : public ot::Dialog {
	Q_OBJECT
	OT_DECL_NOCOPY(RenameProjectDialog)
	OT_DECL_NODEFAULT(RenameProjectDialog)
public:
	RenameProjectDialog(const QString& _projectToRename);
	virtual ~RenameProjectDialog();

	QString getProjectName(void) const;

private Q_SLOTS:
	void slotConfirm(void);

private:
	QString       m_projectToRename;
	ot::LineEdit* m_edit;
	
};
