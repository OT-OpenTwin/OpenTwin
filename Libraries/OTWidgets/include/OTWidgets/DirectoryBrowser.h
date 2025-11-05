// @otlicense
// File: DirectoryBrowser.h
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
#include "OTWidgets/WidgetBase.h"

// Qt header
#include <QtCore/qabstractitemmodel.h>
#include <QtWidgets/qwidget.h>

class QTreeView;
class QFileSystemModel;

namespace ot {

	class LineEdit;

	class OT_WIDGETS_API_EXPORT DirectoryBrowser : public QWidget, public WidgetBase {
		Q_OBJECT
		OT_DECL_NOCOPY(DirectoryBrowser)
		OT_DECL_NOMOVE(DirectoryBrowser)
		OT_DECL_NODEFAULT(DirectoryBrowser)
	public:
		explicit DirectoryBrowser(QWidget* _parent);
		virtual ~DirectoryBrowser();

		virtual QWidget* getQWidget(void) override { return this; };

		virtual const QWidget* getQWidget(void) const override { return this; };

	Q_SIGNALS:
		void currentPathChanged(const QString& _path);

	private Q_SLOTS:
		void slotRootChanged(void);
		void slotSelectionChanged(void);

	private:
		QString getItemText(const QModelIndex& _ix);

		LineEdit* m_rootEdit;
		QTreeView* m_treeView;
		QFileSystemModel* m_model;
		QModelIndex m_rootIx;
	};

}