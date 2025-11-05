// @otlicense
// File: LogVisualizerColumnWidthSaveDialog.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once

// OpenTwin header
#include "OTWidgets/Dialog.h"

class QListWidget;
class QListWidgetItem;
namespace ot { class LineEdit; }

class LogVisualizerColumnWidthSaveDialog : public ot::Dialog {
	Q_OBJECT
	OT_DECL_NOCOPY(LogVisualizerColumnWidthSaveDialog)
	OT_DECL_NOMOVE(LogVisualizerColumnWidthSaveDialog)
	OT_DECL_NODEFAULT(LogVisualizerColumnWidthSaveDialog)
public:
	LogVisualizerColumnWidthSaveDialog(const QStringList& _existingNames, QWidget* _parent);

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