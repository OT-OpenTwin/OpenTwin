//! @file SelectProjectDialog.h
//! @authors Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

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

class QListWidget;

namespace ot {
	class Label;
	class LineEdit;
	class PushButton;
}

class SelectProjectDialogEntry : public QListWidgetItem {
	OT_DECL_NOCOPY(SelectProjectDialogEntry)
	OT_DECL_NOMOVE(SelectProjectDialogEntry)
	OT_DECL_NODEFAULT(SelectProjectDialogEntry)
public:
	SelectProjectDialogEntry(const ot::ProjectInformation& _info);

	const ot::ProjectInformation& getProjectInformation() const { return m_info; };

private:
	ot::ProjectInformation m_info;
};

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

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
	void slotItemDoubleClicked(QListWidgetItem* _item);
	void slotSelectionChanged();
	void slotRefillList();

private:
	QListWidget* m_list;
	ot::PushButton* m_confirmButton;
	ot::LineEdit* m_filter;
	ot::Label* m_infoLabel;
};