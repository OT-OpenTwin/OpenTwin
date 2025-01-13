//! @file FilePathEdit.cpp
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/FilePathEdit.h"

// Qt header
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qfiledialog.h>

ot::FilePathEdit::FilePathEdit(FileMode _mode, QWidget* _parent) 
	: m_mode(_mode), m_root(nullptr), m_edit(nullptr), m_button(nullptr)
{
	this->ini(_parent);
}

ot::FilePathEdit::FilePathEdit(const QString& _filePath, FileMode _mode, QWidget* _parent) 
	: m_mode(_mode), m_file(_filePath), m_root(nullptr), m_edit(nullptr), m_button(nullptr)
{
	this->ini(_parent);
}

ot::FilePathEdit::~FilePathEdit() {
	delete m_root;
}

void ot::FilePathEdit::setFilePath(const QString& _file) {
	m_file = _file;
	m_edit->setText(m_file);
}

void ot::FilePathEdit::slotBrowse(void) {
	QString pth;
	if (m_mode == FilePathEdit::OpenFileMode) {
		pth = QFileDialog::getOpenFileName(m_edit, m_browseTitle, m_edit->text(), m_fileFilter);
	}
	else {
		pth = QFileDialog::getSaveFileName(m_edit, m_browseTitle, m_edit->text(), m_fileFilter);
	}
	if (!pth.isEmpty() && m_edit->text() != pth) {
		m_file = pth;
		m_edit->setText(m_file);
		Q_EMIT fileChanged();
	}
}

void ot::FilePathEdit::slotEditChanged(void) {
	if (m_edit->text() == m_file) return;
	m_file = m_edit->text();
	Q_EMIT fileChanged();
}

void ot::FilePathEdit::ini(QWidget* _parent) {
	m_root = new QWidget(_parent);
	QHBoxLayout* rootLayout = new QHBoxLayout(m_root);
	rootLayout->setContentsMargins(0, 0, 0, 0);

	m_edit = new LineEdit(m_file);

	rootLayout->addWidget(m_edit, 1);

	m_button = new PushButton("Browse");
	rootLayout->addWidget(m_button);

	if (m_mode == OpenFileMode) {
		m_browseTitle = "Open File";
	}
	else {
		m_browseTitle = "Save File";
	}

	this->connect(m_edit, &QLineEdit::editingFinished, this, &FilePathEdit::slotEditChanged);
	this->connect(m_button, &QPushButton::clicked, this, &FilePathEdit::slotBrowse);
}