// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/ProjectInformation.h"
#include "OTWidgets/Dialog.h"

// Qt header
#include <QtGui/qicon.h>
#include <QtWidgets/qlistwidget.h>

// std header
#include <map>
#include <string>

namespace ot {
	class Label;
	class PushButton;
	class ProjectOverviewWidget;
}

class SelectProjectDialog : public ot::Dialog {
	Q_OBJECT
	OT_DECL_NOCOPY(SelectProjectDialog)
	OT_DECL_NOMOVE(SelectProjectDialog)
	OT_DECL_NODEFAULT(SelectProjectDialog)
public:
	SelectProjectDialog(const ot::DialogCfg& _config);
	virtual ~SelectProjectDialog();

	ot::ProjectInformation getSelectedProject() const;

private Q_SLOTS:
	void slotConfirm();
	void slotOpenRequested();
	void slotSelectionChanged();

private:
	ot::ProjectOverviewWidget* m_overview;
	ot::PushButton* m_confirmButton;
	ot::Label* m_infoLabel;
};