//! @file LoggingFilterView.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "LoggingFilterView.h"

// Qt header
#include <QtCore/qjsonarray.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qjsondocument.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qcombobox.h>
#include <QtWidgets/qcheckbox.h>
#include <QtWidgets/qgroupbox.h>
#include <QtWidgets/qlistwidget.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qscrollarea.h>

#define LFV_Filter_AllNames "< All >"

LoggingFilterView::LoggingFilterView() 
	: m_filterTimer(nullptr), m_filterLock(true)
{
	// Create layouts
	QScrollArea* scrollArea = new QScrollArea;
	m_root = scrollArea;
	scrollArea->setObjectName("LoggingFilterViewRoot");

	QWidget* centralLayoutW = new QWidget;
	QVBoxLayout* centralLayout = new QVBoxLayout(centralLayoutW);
	scrollArea->setWidget(centralLayoutW);
	scrollArea->setWidgetResizable(true);

	QHBoxLayout* messageContainsLayout = new QHBoxLayout;

	QGroupBox* filterByMessageTypeBox = new QGroupBox("Message Type");
	QVBoxLayout* filterByMessageTypeLayout = new QVBoxLayout(filterByMessageTypeBox);

	QGroupBox* filterByUserBox = new QGroupBox("User");
	QVBoxLayout* filterByUserLayout = new QVBoxLayout(filterByUserBox);

	QGroupBox* filterByProjectBox = new QGroupBox("Project");
	QVBoxLayout* filterByProjectLayout = new QVBoxLayout(filterByProjectBox);

	QGroupBox* filterByServiceBox = new QGroupBox("Service Type");
	QVBoxLayout* filterByServiceLayout = new QVBoxLayout(filterByServiceBox);

	// Create controls
	QLabel* messageFilterL = new QLabel("Message contains:");
	m_messageFilter = new QLineEdit;
	m_messageFilter->setPlaceholderText("<Confirm by pressing return>");
	
	m_userFilter = new QComboBox;
	m_userFilter->addItem(LFV_Filter_AllNames);
	filterByUserLayout->addWidget(m_userFilter);

	m_sessionFilter = new QComboBox;
	m_sessionFilter->addItem(LFV_Filter_AllNames);
	filterByProjectLayout->addWidget(m_sessionFilter);

	m_msgTypeFilterDetailed = new QCheckBox("Detailed");
	m_msgTypeFilterInfo = new QCheckBox("Info");
	m_msgTypeFilterWarning = new QCheckBox("Warning");
	m_msgTypeFilterError = new QCheckBox("Error");
	m_msgTypeFilterMsgIn = new QCheckBox("Inbound Message");
	m_msgTypeFilterMsgOut = new QCheckBox("Outgoing Message");

	m_serviceFilter = new QListWidget;
	QPushButton* btnSelectAllServices = new QPushButton("Select All");

	QPushButton* btnDeselectAllServices = new QPushButton("Deselect All");

	// Setup controls
	m_msgTypeFilterDetailed->setChecked(true);
	m_msgTypeFilterInfo->setChecked(true);
	m_msgTypeFilterWarning->setChecked(true);
	m_msgTypeFilterError->setChecked(true);

	// Setup layouts
	centralLayout->addLayout(messageContainsLayout);
	centralLayout->addWidget(filterByMessageTypeBox, 0);
	centralLayout->addWidget(filterByUserBox, 0);
	centralLayout->addWidget(filterByProjectBox, 0);
	centralLayout->addWidget(filterByServiceBox, 1);

	messageContainsLayout->addWidget(messageFilterL);
	messageContainsLayout->addWidget(m_messageFilter, 1);

	filterByMessageTypeLayout->addWidget(m_msgTypeFilterDetailed);
	filterByMessageTypeLayout->addWidget(m_msgTypeFilterInfo);
	filterByMessageTypeLayout->addWidget(m_msgTypeFilterWarning);
	filterByMessageTypeLayout->addWidget(m_msgTypeFilterError);
	filterByMessageTypeLayout->addWidget(m_msgTypeFilterMsgIn);
	filterByMessageTypeLayout->addWidget(m_msgTypeFilterMsgOut);

	filterByServiceLayout->addWidget(m_serviceFilter);
	filterByServiceLayout->addWidget(btnSelectAllServices);
	filterByServiceLayout->addWidget(btnDeselectAllServices);

	// Connect signals
	this->connect(m_msgTypeFilterDetailed, &QCheckBox::stateChanged, this, &LoggingFilterView::slotUpdateCheckboxColors);
	this->connect(m_msgTypeFilterInfo, &QCheckBox::stateChanged, this, &LoggingFilterView::slotUpdateCheckboxColors);
	this->connect(m_msgTypeFilterWarning, &QCheckBox::stateChanged, this, &LoggingFilterView::slotUpdateCheckboxColors);
	this->connect(m_msgTypeFilterError, &QCheckBox::stateChanged, this, &LoggingFilterView::slotUpdateCheckboxColors);
	this->connect(m_msgTypeFilterMsgIn, &QCheckBox::stateChanged, this, &LoggingFilterView::slotUpdateCheckboxColors);
	this->connect(m_msgTypeFilterMsgOut, &QCheckBox::stateChanged, this, &LoggingFilterView::slotUpdateCheckboxColors);

	this->connect(m_messageFilter, &QLineEdit::returnPressed, this, &LoggingFilterView::slotFilterChanged);

	this->connect(m_msgTypeFilterDetailed, &QCheckBox::stateChanged, this, &LoggingFilterView::slotFilterChanged);
	this->connect(m_msgTypeFilterInfo, &QCheckBox::stateChanged, this, &LoggingFilterView::slotFilterChanged);
	this->connect(m_msgTypeFilterWarning, &QCheckBox::stateChanged, this, &LoggingFilterView::slotFilterChanged);
	this->connect(m_msgTypeFilterError, &QCheckBox::stateChanged, this, &LoggingFilterView::slotFilterChanged);
	this->connect(m_msgTypeFilterMsgIn, &QCheckBox::stateChanged, this, &LoggingFilterView::slotFilterChanged);
	this->connect(m_msgTypeFilterMsgOut, &QCheckBox::stateChanged, this, &LoggingFilterView::slotFilterChanged);

	this->connect(m_userFilter, &QComboBox::currentTextChanged, this, &LoggingFilterView::slotFilterChanged);
	this->connect(m_sessionFilter, &QComboBox::currentTextChanged, this, &LoggingFilterView::slotFilterChanged);

	this->connect(m_serviceFilter, &QListWidget::itemChanged, this, &LoggingFilterView::slotFilterChanged);

	this->connect(btnSelectAllServices, &QPushButton::clicked, this, &LoggingFilterView::slotSelectAllServices);
	this->connect(btnDeselectAllServices, &QPushButton::clicked, this, &LoggingFilterView::slotDeselectAllServices);
}

LoggingFilterView::~LoggingFilterView() {

}

void LoggingFilterView::reset(void) {
	m_serviceFilter->clear();
}

bool LoggingFilterView::filterMessage(const ot::LogMessage& _msg) {
	bool found = false;
	QString serviceName = QString::fromStdString(_msg.getServiceName());

	// Check Service Name exists
	for (int i = 0; i < m_serviceFilter->count(); i++) {
		if (m_serviceFilter->item(i)->text() == serviceName) {
			found = true;
			break;
		}
	}

	if (!found) {
		bool tmp = m_filterLock;
		m_filterLock = true;
		QListWidgetItem* itm = new QListWidgetItem(serviceName);
		itm->setFlags(itm->flags() | Qt::ItemIsUserCheckable);
		itm->setCheckState(Qt::Checked);
		m_serviceFilter->addItem(itm);
		m_serviceFilter->sortItems(Qt::SortOrder::AscendingOrder);
		m_filterLock = tmp;
	}

	// Check User Name exists
	QString userName = QString::fromStdString(_msg.getUserName());
	if (!userName.isEmpty()) {
		found = false;
		for (int i = 0; i < m_userFilter->count(); i++) {
			if (m_userFilter->itemText(i) == userName) {
				found = true;
				break;
			}
		}

		if (!found) {
			m_filterLock = true;
			m_userFilter->addItem(userName);
			m_filterLock = false;
		}
	}

	// Check Project Name exists
	QString projectName = QString::fromStdString(_msg.getProjectName());
	if (!projectName.isEmpty()) {
		found = false;
		for (int i = 0; i < m_sessionFilter->count(); i++) {
			if (m_sessionFilter->itemText(i) == projectName) {
				found = true;
				break;
			}
		}

		if (!found) {
			m_filterLock = true;
			m_sessionFilter->addItem(projectName);
			m_filterLock = false;
		}
	}

	// Type

	bool typePass = false;
	if (m_msgTypeFilterDetailed->isChecked() && (_msg.getFlags() & ot::DETAILED_LOG)) typePass = true;
	else if (m_msgTypeFilterInfo->isChecked() && (_msg.getFlags() & ot::INFORMATION_LOG)) typePass = true;
	else if (m_msgTypeFilterWarning->isChecked() && (_msg.getFlags() & ot::WARNING_LOG)) typePass = true;
	else if (m_msgTypeFilterError->isChecked() && (_msg.getFlags() & ot::ERROR_LOG)) typePass = true;
	else if (m_msgTypeFilterMsgIn->isChecked() && (_msg.getFlags() & ot::ALL_INCOMING_MESSAGE_LOG_FLAGS)) typePass = true;
	else if (m_msgTypeFilterMsgOut->isChecked() && (_msg.getFlags() & ot::ALL_OUTGOING_MESSAGE_LOG_FLAGS)) typePass = true;

	// User

	bool userPass = true;
	if (m_userFilter->currentText() != LFV_Filter_AllNames) {
		userPass = m_userFilter->currentText() == userName;
	}

	// Project

	bool projectPass = true;
	if (m_sessionFilter->currentText() != LFV_Filter_AllNames) {
		projectPass = m_sessionFilter->currentText() == projectName;
	}

	// Service

	bool servicePass = false;
	for (int i = 0; i < m_serviceFilter->count(); i++) {
		if (m_serviceFilter->item(i)->checkState() == Qt::Checked && m_serviceFilter->item(i)->text() == serviceName) {
			servicePass = true;
			break;
		}
	}

	// Message

	bool messagePass = true;
	QString txt = QString::fromStdString(_msg.getText());
	if (!m_messageFilter->text().isEmpty()) {
		messagePass = txt.contains(m_messageFilter->text(), Qt::CaseInsensitive);
	}

	return typePass && userPass && projectPass && servicePass && messagePass;
}

void LoggingFilterView::restoreSettings(QSettings& _settings) {
	m_msgTypeFilterDetailed->setChecked(_settings.value("LoggingFilterView.FilterActive.Detailed", true).toBool());
	m_msgTypeFilterInfo->setChecked(_settings.value("LoggingFilterView.FilterActive.Info", true).toBool());
	m_msgTypeFilterWarning->setChecked(_settings.value("LoggingFilterView.FilterActive.Warning", true).toBool());
	m_msgTypeFilterError->setChecked(_settings.value("LoggingFilterView.FilterActive.Error", true).toBool());
	m_msgTypeFilterMsgIn->setChecked(_settings.value("LoggingFilterView.FilterActive.Message.In", false).toBool());
	m_msgTypeFilterMsgOut->setChecked(_settings.value("LoggingFilterView.FilterActive.Message.Out", false).toBool());

	QByteArray serviceFilter = _settings.value("LoggingFilterView.ServiceFilter.List", QByteArray()).toByteArray();
	if (!serviceFilter.isEmpty()) {
		QJsonDocument serviceFilterDoc = QJsonDocument::fromJson(serviceFilter);
		if (serviceFilterDoc.isArray()) {
			QJsonArray serviceFilterArr = serviceFilterDoc.array();

			for (int i = 0; i < serviceFilterArr.count(); i++) {
				if (serviceFilterArr[i].isObject()) {
					QJsonObject serviceObj = serviceFilterArr[i].toObject();

					if (serviceObj.contains("Name") && serviceObj.contains("Active")) {
						if (serviceObj["Name"].isString() && serviceObj["Active"].isBool()) {
							QListWidgetItem* nItm = new QListWidgetItem(serviceObj["Name"].toString());
							nItm->setFlags(nItm->flags() | Qt::ItemIsUserCheckable);
							nItm->setCheckState((serviceObj["Active"].toBool() ? Qt::Checked : Qt::Unchecked));
							m_serviceFilter->addItem(nItm);
						}
					}
				}
			}
			m_serviceFilter->sortItems(Qt::SortOrder::AscendingOrder);
		}
	}

	this->slotUpdateCheckboxColors();
}

void LoggingFilterView::saveSettings(QSettings& _settings) {
	_settings.setValue("LoggingFilterView.FilterActive.Detailed", m_msgTypeFilterDetailed->isChecked());
	_settings.setValue("LoggingFilterView.FilterActive.Info", m_msgTypeFilterInfo->isChecked());
	_settings.setValue("LoggingFilterView.FilterActive.Warning", m_msgTypeFilterWarning->isChecked());
	_settings.setValue("LoggingFilterView.FilterActive.Error", m_msgTypeFilterError->isChecked());
	_settings.setValue("LoggingFilterView.FilterActive.Message.In", m_msgTypeFilterMsgIn->isChecked());
	_settings.setValue("LoggingFilterView.FilterActive.Message.Out", m_msgTypeFilterMsgOut->isChecked());

	QJsonArray serviceFilterArr;
	for (int i = 0; i < m_serviceFilter->count(); i++) {
		QJsonObject serviceObj;
		serviceObj["Name"] = m_serviceFilter->item(i)->text();
		serviceObj["Active"] = (m_serviceFilter->item(i)->checkState() == Qt::Checked);
		serviceFilterArr.push_back(serviceObj);
	}
	QJsonDocument serviceFilterDoc(serviceFilterArr);
	_settings.setValue("LoggingFilterView.ServiceFilter.List", QVariant(serviceFilterDoc.toJson(QJsonDocument::Compact)));

}

void LoggingFilterView::slotUpdateCheckboxColors(void) {
	QString red("QCheckBox { color: #c02020; }");
	QString green("QCheckBox { color: #20c020; }");

	m_msgTypeFilterDetailed->setStyleSheet(m_msgTypeFilterDetailed->isChecked() ? green : red);
	m_msgTypeFilterInfo->setStyleSheet(m_msgTypeFilterInfo->isChecked() ? green : red);
	m_msgTypeFilterWarning->setStyleSheet(m_msgTypeFilterWarning->isChecked() ? green : red);
	m_msgTypeFilterError->setStyleSheet(m_msgTypeFilterError->isChecked() ? green : red);
	m_msgTypeFilterMsgIn->setStyleSheet(m_msgTypeFilterMsgIn->isChecked() ? green : red);
	m_msgTypeFilterMsgOut->setStyleSheet(m_msgTypeFilterMsgOut->isChecked() ? green : red);
}

void LoggingFilterView::slotFilterChanged(void) {
	if (m_filterLock) return;
	Q_EMIT filterChanged();
}

void LoggingFilterView::slotSelectAllServices(void) {
	m_filterLock = true;
	for (int i = 0; i < m_serviceFilter->count(); i++) m_serviceFilter->item(i)->setCheckState(Qt::Checked);
	m_filterLock = false;
	this->slotFilterChanged();
}

void LoggingFilterView::slotDeselectAllServices(void) {
	m_filterLock = true;
	for (int i = 0; i < m_serviceFilter->count(); i++) m_serviceFilter->item(i)->setCheckState(Qt::Unchecked);
	m_filterLock = false;
	this->slotFilterChanged();
}
