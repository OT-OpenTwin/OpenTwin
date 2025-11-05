// @otlicense
// File: PathBrowseEdit.cpp
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

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/PathBrowseEdit.h"

// Qt header
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qfiledialog.h>

ot::PathBrowseEdit::PathBrowseEdit(PathBrowseMode _mode, QWidget* _parent) : PathBrowseEdit(QString(), _mode, _parent) {}

ot::PathBrowseEdit::PathBrowseEdit(const QString& _path, PathBrowseMode _mode, QWidget* _parent) :
	m_mode(_mode), m_path(_path), m_root(nullptr), m_edit(nullptr), m_button(nullptr)
{
	m_root = new QWidget(_parent);
	m_root->setContentsMargins(0, 0, 0, 0);

	QHBoxLayout* rootLayout = new QHBoxLayout(m_root);
	rootLayout->setContentsMargins(0, 0, 0, 0);

	m_edit = new LineEdit(m_path, m_root);

	rootLayout->addWidget(m_edit, 1);

	m_button = new PushButton("Browse", m_root);
	rootLayout->addWidget(m_button);

	this->setBrowseMode(m_mode);

	this->connect(m_edit, &QLineEdit::editingFinished, this, &PathBrowseEdit::slotEditChanged);
	this->connect(m_button, &QPushButton::clicked, this, &PathBrowseEdit::slotBrowse);
}

ot::PathBrowseEdit::~PathBrowseEdit() {
	delete m_root;
}

void ot::PathBrowseEdit::setPath(const QString& _path) {
	m_path = _path;
	m_edit->setText(m_path);
}

void ot::PathBrowseEdit::setBrowseMode(PathBrowseMode _mode) {
	m_mode = _mode;

	switch (m_mode) {
	case ot::PathBrowseMode::ReadFile:
		m_browseTitle = "Open File";
		break;
	case ot::PathBrowseMode::WriteFile:
		m_browseTitle = "Save File";
		break;
	case ot::PathBrowseMode::Directory:
		m_browseTitle = "Open Directory";
		break;
	default:
		OT_LOG_EA("Unknown browse mode");
		break;
	}
}

void ot::PathBrowseEdit::slotBrowse(void) {
	QString pth;

	switch (m_mode) {
	case PathBrowseMode::ReadFile:
		pth = QFileDialog::getOpenFileName(m_edit, m_browseTitle, m_edit->text(), m_fileFilter);
		break;
	case PathBrowseMode::WriteFile:
		pth = QFileDialog::getSaveFileName(m_edit, m_browseTitle, m_edit->text(), m_fileFilter);
		break;
	case PathBrowseMode::Directory:
		pth = QFileDialog::getExistingDirectory(m_edit, m_browseTitle, m_edit->text(), QFileDialog::ShowDirsOnly);
		break;
	default:
		OT_LOG_EA("Unknown browse mode");
		break;
	}

	if (!pth.isEmpty() && m_edit->text() != pth) {
		m_path = pth;
		m_edit->setText(m_path);

		Q_EMIT pathChanged();
	}
}

void ot::PathBrowseEdit::slotEditChanged(void) {
	if (m_edit->text() == m_path) {
		return;
	}

	m_path = m_edit->text();

	Q_EMIT pathChanged();
}
