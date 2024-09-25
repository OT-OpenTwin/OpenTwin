//! @file CopyProjectDialog.h
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/Dialog.h"

namespace ot { class LineEdit; };

class CopyProjectDialog : public ot::Dialog {
	Q_OBJECT
	OT_DECL_NOCOPY(CopyProjectDialog)
	OT_DECL_NODEFAULT(CopyProjectDialog)
public:
	CopyProjectDialog(const QString& _projectToCopy);
	virtual ~CopyProjectDialog();

	QString getProjectName(void) const;

private Q_SLOTS:
	void slotConfirm(void);
	
private:
	QString       m_projectToCopy;
	ot::LineEdit* m_edit;
};

