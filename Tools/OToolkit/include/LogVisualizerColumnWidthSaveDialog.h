// @otlicense

//! @file LogVisualizerColumnWidthSaveDialog.h
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/Dialog.h"

class QListWidget;
class QListWidgetItem;
namespace ot { class LineEdit; }

class LogVisualizerColumnWidthSaveDialog : public ot::Dialog {
	Q_OBJECT
public:
	LogVisualizerColumnWidthSaveDialog(const QStringList& _existingNames);

	QStringList getExistingNames() const;
	QString getNewName() const;

private Q_SLOTS:
	void slotSave();
	void slotItemDoubleClicked(QListWidgetItem* _item);
	void slotItemSelectionChanged();
	void slotContextMenuRequested(const QPoint& _pos);

private:
	QListWidget* m_names;
	ot::LineEdit* m_name;

};