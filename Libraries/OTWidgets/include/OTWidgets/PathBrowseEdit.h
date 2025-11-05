// @otlicense
// File: PathBrowseEdit.h
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
#include <QtCore/qobject.h>

class QWidget;

namespace ot {

	class LineEdit;
	class PushButton;

	class OT_WIDGETS_API_EXPORT PathBrowseEdit : public QObject, public WidgetBase {
		Q_OBJECT
		OT_DECL_NOCOPY(PathBrowseEdit)
		OT_DECL_NOMOVE(PathBrowseEdit)
		OT_DECL_NODEFAULT(PathBrowseEdit)
	public:
		explicit PathBrowseEdit(PathBrowseMode _mode, QWidget* _parent);
		explicit PathBrowseEdit(const QString& _path, PathBrowseMode _mode, QWidget* _parent);
		virtual ~PathBrowseEdit();

		virtual QWidget* getQWidget(void) override { return m_root; };
		virtual const QWidget* getQWidget(void) const override { return m_root; };

		void setPath(const QString& _path);
		const QString& getPath(void) const { return m_path; };

		void setBrowseMode(PathBrowseMode _mode);
		PathBrowseMode getBrowseMode(void) const { return m_mode; };

		void setBrowseTitle(const QString& _title) { m_browseTitle = _title; };
		const QString& getBrowseTitle(void) const { return m_browseTitle; };

		void setFileFilter(const QString& _filter) { m_fileFilter = _filter; };
		const QString& getFileFilter(void) const { return m_fileFilter; };

		LineEdit* getLineEdit(void) { return m_edit; };
		PushButton* getPushButton(void) { return m_button; };

	Q_SIGNALS:
		void pathChanged(void);

	public Q_SLOTS:
		void slotBrowse(void);

	private Q_SLOTS:
		void slotEditChanged(void);

	private:
		QString m_path;

		PathBrowseMode m_mode;
		QWidget* m_root;
		LineEdit* m_edit;
		PushButton* m_button;

		QString m_browseTitle;
		QString m_fileFilter;
	};

}