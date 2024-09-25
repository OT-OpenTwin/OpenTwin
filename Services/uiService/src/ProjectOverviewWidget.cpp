//! @file ProjectOverviewWidget.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

// Frontend header
#include "ProjectOverviewWidget.h"

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/Label.h"
#include "OTWidgets/Table.h"
#include "OTWidgets/CheckBox.h"
#include "OTWidgets/LineEdit.h"

// Qt header
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qheaderview.h>
#include <QtOpenGLWidgets/qopenglwidget.h>

enum TableColumn {
	ColumnCheck,
	ColumnName,
	ColumnOwner,
	ColumnCount
};

ProjectOverviewEntry::ProjectOverviewEntry(const QString& _projectName, const QString& _owner, QTableWidget* _table) {
	m_row = _table->rowCount();
	_table->insertRow(m_row);

	m_checkBox = new ot::CheckBox;
	
	m_nameItem = new QTableWidgetItem;
	m_nameItem->setFlags(Qt::ItemIsSelectable);
	m_nameItem->setText(_projectName);

	m_ownerItem = new QTableWidgetItem;
	m_ownerItem->setFlags(Qt::ItemIsSelectable);
	m_ownerItem->setText(_owner);

	_table->setCellWidget(m_row, TableColumn::ColumnCheck, m_checkBox);
	_table->setItem(m_row, TableColumn::ColumnName, m_nameItem);
	_table->setItem(m_row, TableColumn::ColumnOwner, m_ownerItem);

	this->connect(m_checkBox, &ot::CheckBox::stateChanged, this, &ProjectOverviewEntry::slotCheckedChanged);
}

bool ProjectOverviewEntry::getIsChecked(void) const {
	return m_checkBox->isChecked();
}

QString ProjectOverviewEntry::getProjectName(void) const {
	return m_nameItem->text();
}

void ProjectOverviewEntry::slotCheckedChanged(void) {
	Q_EMIT checkedChanged(m_row);
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ProjectOverviewWidget::ProjectOverviewWidget() 
	: m_lockCount(0)
{
	// Create layouts
	m_widget = new QWidget;
	QVBoxLayout* centralLayout = new QVBoxLayout(m_widget);

	// Crete controls
	ot::Label* welcomeLabel = new ot::Label("Welcome");
	m_filter = new ot::LineEdit;
	m_table = new ot::Table(0, TableColumn::ColumnCount);
	m_countLabel = new ot::Label;
	QOpenGLWidget* glWidget = new QOpenGLWidget;

	// Setup controls
	QFont welcomeFont = welcomeLabel->font();
	welcomeFont.setPixelSize(48);
	welcomeLabel->setFont(welcomeFont);

	m_filter->setPlaceholderText("Find...");
	m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	m_table->horizontalHeader()->setSectionResizeMode(TableColumn::ColumnName, QHeaderView::Stretch);
	m_table->setHorizontalHeaderLabels({ "", "Name", "Owner" });
	m_table->verticalHeader()->setHidden(true);

	glWidget->setMaximumSize(1, 1);

	this->updateCountLabel();

	// Setup layouts
	centralLayout->addWidget(welcomeLabel);
	centralLayout->addWidget(m_filter);
	centralLayout->addWidget(m_table);
	centralLayout->addWidget(m_countLabel);
	centralLayout->addWidget(glWidget);

	// Connect signals
	this->connect(m_filter, &ot::LineEdit::textChanged, this, &ProjectOverviewWidget::slotFilterChanged);
}

ProjectOverviewWidget::~ProjectOverviewWidget() {

}

void ProjectOverviewWidget::lock(bool _flag) {
	if (_flag) {
		if (m_lockCount == 0) {
			m_widget->setEnabled(false);
		}
		m_lockCount++;
	}
	else if (m_lockCount == 0) {
		OTAssert(0, "No lock set, check locks");
		return;
	}
	else {
		m_lockCount--;
		if (m_lockCount == 0) {
			m_widget->setEnabled(true);
		}
	}
}

void ProjectOverviewWidget::slotRefreshProjectList(void) {

}

void ProjectOverviewWidget::slotRefreshRecentProjects(void) {

}

void ProjectOverviewWidget::slotRefreshAllProjects(void) {

}

void ProjectOverviewWidget::slotFilterChanged(void) {
	if (m_filter->text().isEmpty()) {
		for (int r = 0; r < m_table->rowCount(); r++) {
			m_table->setRowHidden(r, false);
		}
	}
	else {
		for (int r = 0; r < m_table->rowCount(); r++) {
			OTAssert(r < m_entries.size(), "Index mismatch");
			m_table->setRowHidden(r, !(m_entries[r]->getProjectName().contains(m_filter->text(), Qt::CaseInsensitive)));
		}
	}
}

void ProjectOverviewWidget::slotProjectCheckedChanged(int _row) {

}

void ProjectOverviewWidget::clear(void) {
	for (ProjectOverviewEntry* entry : m_entries) {
		this->disconnect(entry, &ProjectOverviewEntry::checkedChanged, this, &ProjectOverviewWidget::slotProjectCheckedChanged);
		delete entry;
	}
	m_entries.clear();
	m_table->setRowCount(0);
}

void ProjectOverviewWidget::addProject(const QString& _projectName, const QString& _owner) {
	ProjectOverviewEntry* newEntry = new ProjectOverviewEntry(_projectName, _owner, m_table);
	OTAssert(newEntry->getRow() == m_entries.size(), "Index mismatch");
	m_entries.push_back(newEntry);
	this->connect(newEntry, &ProjectOverviewEntry::checkedChanged, this, &ProjectOverviewWidget::slotProjectCheckedChanged);
}

void ProjectOverviewWidget::updateCountLabel(void) {
	if (m_entries.empty()) {
		m_countLabel->setText("No projects found");
	}
	else if (m_entries.size() == 1) {
		m_countLabel->setText("1 project found");
	}
	else {
		m_countLabel->setText(QString::number(m_entries.size()) + " projects found");
	}
}
