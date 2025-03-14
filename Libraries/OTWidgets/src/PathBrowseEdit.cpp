//! @file PathBrowseEdit.cpp
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/PathBrowseEdit.h"

// Qt header
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qfiledialog.h>

ot::PathBrowseEdit::PathBrowseEdit(PathBrowseMode _mode, QWidget* _parent)
	: m_mode(_mode), m_root(nullptr), m_edit(nullptr), m_button(nullptr)
{
	this->ini(_parent);
}

ot::PathBrowseEdit::PathBrowseEdit(const QString& _path, PathBrowseMode _mode, QWidget* _parent)
	: m_mode(_mode), m_path(_path), m_root(nullptr), m_edit(nullptr), m_button(nullptr)
{
	this->ini(_parent);
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

void ot::PathBrowseEdit::ini(QWidget* _parent) {
	m_root = new QWidget(_parent);
	m_root->setContentsMargins(0, 0, 0, 0);

	QHBoxLayout* rootLayout = new QHBoxLayout(m_root);
	rootLayout->setContentsMargins(0, 0, 0, 0);

	m_edit = new LineEdit(m_path);

	rootLayout->addWidget(m_edit, 1);

	m_button = new PushButton("Browse");
	rootLayout->addWidget(m_button);

	this->setBrowseMode(m_mode);

	this->connect(m_edit, &QLineEdit::editingFinished, this, &PathBrowseEdit::slotEditChanged);
	this->connect(m_button, &QPushButton::clicked, this, &PathBrowseEdit::slotBrowse);
}