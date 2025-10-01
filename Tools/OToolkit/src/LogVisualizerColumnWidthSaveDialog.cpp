//! @file LogVisualizerColumnWidthSaveDialog.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "LogVisualizerColumnWidthSaveDialog.h"

// OpenTwin header
#include "OTWidgets/Label.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/BasicValidator.h"

// Qt header
#include <QtWidgets/qmenu.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qtooltip.h>
#include <QtWidgets/qlistwidget.h>
#include <QtWidgets/qmessagebox.h>

LogVisualizerColumnWidthSaveDialog::LogVisualizerColumnWidthSaveDialog(const QStringList& _existingNames) {
	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	QVBoxLayout* existingLayout = new QVBoxLayout;
	existingLayout->addWidget(new ot::Label("Existing Configurations:"));
	m_names = new QListWidget;
	m_names->setSelectionMode(QAbstractItemView::SingleSelection);
	m_names->setContextMenuPolicy(Qt::CustomContextMenu);
	this->connect(m_names, &QListWidget::itemSelectionChanged, this, &LogVisualizerColumnWidthSaveDialog::slotItemSelectionChanged);
	this->connect(m_names, &QListWidget::itemDoubleClicked, this, &LogVisualizerColumnWidthSaveDialog::slotItemDoubleClicked);
	this->connect(m_names, &QListWidget::customContextMenuRequested, this, &LogVisualizerColumnWidthSaveDialog::slotContextMenuRequested);

	for (const QString& n : _existingNames) {
		m_names->addItem(n);
	}
	existingLayout->addWidget(m_names);
	mainLayout->addLayout(existingLayout, 1);

	QHBoxLayout* nameLayout = new QHBoxLayout;
	nameLayout->addWidget(new ot::Label("New Name:"));
	m_name = new ot::LineEdit;
	m_name->setValidator(new ot::BasicValidator);
	nameLayout->addWidget(m_name, 1);
	mainLayout->addLayout(nameLayout);

	QHBoxLayout* buttonLayout = new QHBoxLayout;
	buttonLayout->addStretch(1);

	ot::PushButton* btnSave = new ot::PushButton("Save");
	buttonLayout->addWidget(btnSave);
	this->connect(btnSave, &QPushButton::clicked, this, &LogVisualizerColumnWidthSaveDialog::slotSave);

	ot::PushButton* btnCancel = new ot::PushButton("Cancel");
	buttonLayout->addWidget(btnCancel);
	this->connect(btnCancel, &QPushButton::clicked, this, &LogVisualizerColumnWidthSaveDialog::closeCancel);
	mainLayout->addLayout(buttonLayout);

	this->resize(400, 300);
	this->setWindowTitle("Save Column Width Configuration");
	this->setWindowIcon(ot::IconManager::getApplicationIcon());
}

QStringList LogVisualizerColumnWidthSaveDialog::getExistingNames() const {
	QStringList names;
	for (int i = 0; i < m_names->count(); i++) {
		names.push_back(m_names->item(i)->text());
	}
	return names;
}

QString LogVisualizerColumnWidthSaveDialog::getNewName() const {
	return m_name->text();
}

void LogVisualizerColumnWidthSaveDialog::slotSave() {
	if (m_name->text().isEmpty()) {
		QToolTip::showText(m_name->mapToGlobal(QPoint(0, m_name->height())), "Please provide a name for the configuration", m_name);
		return;
	}

	for (int i = 0; i < m_names->count(); i++) {
		if (m_names->item(i)->text().toLower() == m_name->text().toLower()) {
			if (QMessageBox::question(this, "Overwrite existing configuration?", QString("A configuration with the name \"" + m_name->text() + "\" already exists. Do you want to overwrite it?"), QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) {				
				return;
			}
			else {
				break;
			}
		}
	}

	this->closeOk();
}

void LogVisualizerColumnWidthSaveDialog::slotItemDoubleClicked(QListWidgetItem* _item) {
	if (_item == nullptr) {
		return;
	}
	m_name->setText(_item->text());
	this->closeOk();
}

void LogVisualizerColumnWidthSaveDialog::slotItemSelectionChanged() {
	if (m_names->selectedItems().empty()) {
		return;
	}
	else {
		m_name->setText(m_names->selectedItems().first()->text());
	}
}

void LogVisualizerColumnWidthSaveDialog::slotContextMenuRequested(const QPoint& _pos) {
	QListWidgetItem* item = m_names->itemAt(_pos);
	if (item == nullptr) {
		return;
	}

	QMenu menu(this);
	QAction* deleteAction = menu.addAction("Delete");
	menu.exec(m_names->mapToGlobal(_pos));

	if (menu.activeAction() == deleteAction) {
		delete item;
	}
}
