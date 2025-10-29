// @otlicense

//! @file LogServiceDebugInfo.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "LogServiceDebugInfo.h"
#include "LogServiceDebugInfoDialog.h"
#include "OToolkitAPI/OToolkitAPI.h"

// OpenTwin header
#include "OTWidgets/Table.h"
#include "OTWidgets/TableItem.h"
#include "OTWidgets/PushButton.h"

// Qt header
#include <QtGui/qclipboard.h>
#include <QtGui/qguiapplication.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qheaderview.h>

#define LOGVIS_LOG(___message) OTOOLKIT_LOG("Service Info Viewer", ___message)
#define LOGVIS_LOGW(___message) OTOOLKIT_LOGW("Service Info Viewer", ___message)
#define LOGVIS_LOGE(___message) OTOOLKIT_LOGE("Service Info Viewer", ___message)

enum TableColumns {
	tcName,
	tcUrl,
	tcID,
	tcPID,
	tcAdditionalInfo,
	tcCount
};

LogServiceDebugInfo::LogServiceDebugInfo() {
	m_root = new QWidget;
	QVBoxLayout* mainLayout = new QVBoxLayout(m_root);

	m_table = new ot::Table(0, tcCount);
	m_table->verticalHeader()->setVisible(false);
	m_table->setSortingEnabled(true);
	m_table->horizontalHeader()->setSortIndicatorClearable(true);
	m_table->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
	m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_table->setAlternatingRowColors(true);

	m_table->setHorizontalHeaderLabels({ "Name", "URL", "ID", "PID", "Info" });
	m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
	m_table->horizontalHeader()->setSectionResizeMode(tcAdditionalInfo, QHeaderView::ResizeMode::Stretch);

	mainLayout->addWidget(m_table);

	ot::PushButton* clearButton = new ot::PushButton("Clear");
	QHBoxLayout* buttonLayout = new QHBoxLayout;
	buttonLayout->setContentsMargins(0, 0, 0, 0);
	buttonLayout->addStretch(1);
	buttonLayout->addWidget(clearButton, 0);
	mainLayout->addLayout(buttonLayout);

	this->connect(clearButton, &ot::PushButton::clicked, this, &LogServiceDebugInfo::slotClear);
	this->connect(m_table, &QTableWidget::itemDoubleClicked, this, &LogServiceDebugInfo::slotItemDoubleClicked);
}

LogServiceDebugInfo::~LogServiceDebugInfo() {

}

void LogServiceDebugInfo::restoreSettings(QSettings& _settings) {

}

void LogServiceDebugInfo::saveSettings(QSettings& _settings) {

}

void LogServiceDebugInfo::appendServiceDebugInfo(const ot::ServiceDebugInformation& _info) {
	int r = m_table->rowCount();

	QString name = QString::fromStdString(_info.getServiceName());
	QString url = QString::fromStdString(_info.getServiceUrl());
	QString id = (_info.getServiceID() == ot::invalidServiceID ? QString() : QString::number(_info.getServiceID()));
	QString pid = QString::number(_info.getProcessID());

	for (int i = 0; i < r; i++) {
		if (m_table->item(i, tcName)->text() == name &&
			m_table->item(i, tcUrl)->text() == url &&
			m_table->item(i, tcID)->text() == id &&
			m_table->item(i, tcPID)->text() == pid)
		{
			ot::JsonDocument doc;
			_info.addToJsonObject(doc, doc.GetAllocator());
			LOGVIS_LOGW("Duplicate service information received: " + QString::fromStdString(doc.toJson()));
			return;
		}
	}

	m_table->setSortingEnabled(false);

	m_table->insertRow(r);

	const Qt::ItemFlags itemFlags = Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled;

	m_table->addItem(r, tcName, name)->setFlags(itemFlags);
	m_table->addItem(r, tcUrl, url)->setFlags(itemFlags);
	m_table->addItem(r, tcID, id)->setFlags(itemFlags);
	m_table->addItem(r, tcPID, pid)->setFlags(itemFlags);
	m_table->addItem(r, tcAdditionalInfo, QString::fromStdString(_info.getAdditionalInformationJson()))->setFlags(itemFlags);

	m_table->setSortingEnabled(true);
}

void LogServiceDebugInfo::slotItemDoubleClicked(QTableWidgetItem* _item) {
	QString name = m_table->item(_item->row(), tcName)->text();
	QString url = m_table->item(_item->row(), tcUrl)->text();
	QString id = m_table->item(_item->row(), tcID)->text();
	QString pid = m_table->item(_item->row(), tcPID)->text();
	QString info = m_table->item(_item->row(), tcAdditionalInfo)->text();

	if (_item->column() == tcUrl) {
		QGuiApplication::clipboard()->setText(_item->text());
		LOGVIS_LOG("Copied URL of service \"" + name + "\" to clipboard: " + url);
	}
	else {
		LogServiceDebugInfoDialog dia(name, url, id, pid, info, m_root);
		dia.showDialog();
	}
}

void LogServiceDebugInfo::slotClear() {
	m_table->setRowCount(0);
}
