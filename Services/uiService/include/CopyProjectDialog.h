// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/Dialog.h"

namespace ot { class LineEdit; };

class ProjectManagement;

class CopyProjectDialog : public ot::Dialog {
	Q_OBJECT
	OT_DECL_NOCOPY(CopyProjectDialog)
	OT_DECL_NODEFAULT(CopyProjectDialog)
public:
	CopyProjectDialog(const QString& _projectToCopy, ProjectManagement &projectManager);
	virtual ~CopyProjectDialog();

	QString getProjectName(void) const;

private Q_SLOTS:
	void slotConfirm(void);
	
private:
	QString       m_projectToCopy;
	ot::LineEdit* m_edit;
	ProjectManagement *m_projectManagement;
};

