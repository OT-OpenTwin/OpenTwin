//! @file Randomizer.cpp
//! @author Alexander Kuester (alexk95)
//! @date December 2023
// ###########################################################################################################################################################################################################################################################################################################################

// Toolkit header
#include "Randomizer.h"
#include "OToolkitAPI/OToolkitAPI.h"

// Qt header
#include <QtCore/qjsonarray.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qjsondocument.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qspinbox.h>
#include <QtWidgets/qtooltip.h>
#include <QtWidgets/qgroupbox.h>
#include <QtWidgets/qcheckbox.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qtabwidget.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qradiobutton.h>
#include <QtWidgets/qplaintextedit.h>

// std header
#include <vector>

class QLabel;
class QWidget;
class QGroupBox;
class QCheckBox;
class QLineEdit;
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QRadioButton;
class QNumericUpDown;

#define RAND_LOG(___message) OTOOLKIT_LOG("Randomizer", ___message)
#define RAND_LOGW(___message) OTOOLKIT_LOGW("Randomizer", ___message)
#define RAND_LOGE(___message) OTOOLKIT_LOGE("Randomizer", ___message)

Randomizer::Randomizer() {

}

Randomizer::~Randomizer() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// API base functions

bool Randomizer::runTool(QMenu* _rootMenu, otoolkit::ToolWidgets& _content) {
	// Root
	QWidget* rootW = new QWidget;
	m_root = this->createCentralWidgetView(rootW, "Randomizer");
	_content.addView(m_root);

	m_rootLayout = new QVBoxLayout(rootW);
	m_leftTitle = new QLabel("Randomizer type");

	m_rootLayout->addWidget(m_leftTitle);

	// Bool
	m_boolGroup = new QGroupBox("Boolean");
	m_boolLayout = new QVBoxLayout(m_boolGroup);
	m_boolTypeLayout = new QHBoxLayout;
	m_zeroOneRB = new QRadioButton("0/1");
	m_boolRB = new QRadioButton("true/false");
	m_yesNoRB = new QRadioButton("Yes/No");
	m_boolResult = new QLabel("<Result>");
	m_boolRun = new QPushButton("Run");

	m_boolTypeLayout->addWidget(m_zeroOneRB);
	m_boolTypeLayout->addWidget(m_boolRB);
	m_boolTypeLayout->addWidget(m_yesNoRB);
	m_boolTypeLayout->addStretch(1);
	m_boolLayout->addLayout(m_boolTypeLayout);
	m_boolLayout->addWidget(m_boolResult, 1, Qt::AlignCenter);
	m_boolLayout->addWidget(m_boolRun, Qt::AlignCenter);

	m_rootLayout->addWidget(m_boolGroup);

	this->connect(m_boolRun, &QPushButton::clicked, this, &Randomizer::slotRunBool);

	// Text
	m_textGroup = new QGroupBox("Text");
	m_textLayout = new QVBoxLayout(m_textGroup);
	m_textTopLayout = new QHBoxLayout;
	m_upperCaseCB = new QCheckBox("\"abc\"");
	m_lowerCaseCB = new QCheckBox("\"ABC\"");
	m_numbersCB = new QCheckBox("\"012\"");
	m_symbolsCB = new QCheckBox("Symbols:");
	m_symbolsEdit = new QLineEdit;
	m_textBottomLayout = new QHBoxLayout;
	m_textLengthL = new QLabel("Text length:");
	m_textLength = new QSpinBox;
	m_textLength->setRange(1, 1024);
	m_textRun = new QPushButton("     Run     ");
	m_textResult = new QLineEdit;
	m_textResult->setReadOnly(true);

	m_textTopLayout->addWidget(m_upperCaseCB);
	m_textTopLayout->addWidget(m_lowerCaseCB);
	m_textTopLayout->addWidget(m_numbersCB);
	m_textTopLayout->addWidget(m_symbolsCB);
	m_textTopLayout->addWidget(m_symbolsEdit, 1);
	m_textLayout->addLayout(m_textTopLayout);
	m_textBottomLayout->addWidget(m_textLengthL);
	m_textBottomLayout->addWidget(m_textLength);
	m_textBottomLayout->addWidget(m_textRun);
	m_textBottomLayout->addStretch(1);
	m_textLayout->addLayout(m_textBottomLayout);
	m_textLayout->addWidget(m_textResult);
	m_rootLayout->addWidget(m_textGroup);

	this->connect(m_textRun, &QPushButton::clicked, this, &Randomizer::slotRunText);

	// List
	m_listGroup = new QGroupBox("List");
	m_listLayout = new QVBoxLayout(m_listGroup);
	m_listTabs = new QTabWidget;
	m_addListLayout = new QHBoxLayout;
	m_addListBtn = new QPushButton("Add");
	m_runList = new QPushButton("Run");
	m_listResult = new QLineEdit;
	m_listResult->setReadOnly(true);

	m_addListLayout->addWidget(m_addListBtn);
	m_addListLayout->addStretch(1);

	m_listLayout->addWidget(m_listTabs, 1);
	m_listLayout->addLayout(m_addListLayout);
	m_listLayout->addWidget(m_runList);
	m_listLayout->addWidget(m_listResult);

	

	m_rootLayout->addWidget(m_listGroup, 1);

	this->connect(m_addListBtn, &QPushButton::clicked, this, &Randomizer::slotAddList);
	this->connect(m_runList, &QPushButton::clicked, this, &Randomizer::slotRunList);

	return true;
}

void Randomizer::restoreToolSettings(QSettings& _settings) {
	m_zeroOneRB->setChecked(_settings.value("Rand.BZO", false).toBool());
	m_boolRB->setChecked(_settings.value("Rand.BTF", true).toBool());
	m_yesNoRB->setChecked(_settings.value("Rand.BYN", false).toBool());
	m_upperCaseCB->setChecked(_settings.value("Rand.TUP", true).toBool());
	m_lowerCaseCB->setChecked(_settings.value("Rand.TDN", true).toBool());
	m_numbersCB->setChecked(_settings.value("Rand.TNU", true).toBool());
	m_symbolsCB->setChecked(_settings.value("Rand.TSY", true).toBool());
	m_symbolsEdit->setText(_settings.value("Rand.SYM", QString("~'! @#$%^&*()_-+={}[]|\\:;\"<,>.?/")).toString());
	m_textLength->setValue(_settings.value("Rand.TLE", 16).toInt());

	QJsonParseError err;
	QJsonDocument doc = QJsonDocument::fromJson(_settings.value("Rand.LST", QByteArray()).toByteArray(), &err);

	if (err.error == QJsonParseError::NoError && doc.isArray()) {
		const QJsonArray arr = doc.array();
		for (int i = 0; i < arr.count(); i++) {
			if (!arr[i].isObject()) {
				RAND_LOGE("List member not an object");
				break;
			}
			const QJsonObject obj = arr[i].toObject();
			if (!obj.contains("Name")) {
				RAND_LOGE("List name member missing");
				break;
			}
			if (!obj.contains("Text")) {
				RAND_LOGE("List text member missing");
				break;
			}
			if (!obj["Name"].isString()) {
				RAND_LOGE("List name member is not a string");
				break;
			}
			if (!obj["Text"].isString()) {
				RAND_LOGE("List text member is not a string");
				break;
			}
			QString n = obj["Name"].toString();
			QPlainTextEdit* e = new QPlainTextEdit(obj["Text"].toString());
			m_lists.insert_or_assign(n, e);
			m_listTabs->addTab(e, n);
		}
	}
	else {
		RAND_LOGE(err.errorString());
	}
}

bool Randomizer::prepareToolShutdown(QSettings& _settings) {
	_settings.setValue("Rand.BZO", m_zeroOneRB->isChecked());
	_settings.setValue("Rand.BTF", m_boolRB->isChecked());
	_settings.setValue("Rand.BYN", m_yesNoRB->isChecked());

	_settings.setValue("Rand.TUP", m_upperCaseCB->isChecked());
	_settings.setValue("Rand.TDN", m_lowerCaseCB->isChecked());
	_settings.setValue("Rand.TNU", m_numbersCB->isChecked());
	_settings.setValue("Rand.TSY", m_symbolsCB->isChecked());
	_settings.setValue("Rand.SYM", m_symbolsEdit->text());
	_settings.setValue("Rand.TLE", m_textLength->value());

	QJsonArray arr;
	for (auto lst : m_lists) {
		QJsonObject obj;
		obj["Name"] = lst.first;
		obj["Text"] = lst.second->toPlainText();
		arr.append(obj);
	}

	QJsonDocument doc(arr);
	_settings.setValue("Rand.LST", doc.toJson());

	return true;
}

void Randomizer::slotRunBool(void) {
	bool res = rand() % 2;
	if (m_zeroOneRB->isChecked()) {
		m_boolResult->setText(res ? "1" : "0");
	} else if (m_boolRB->isChecked()) {
		m_boolResult->setText(res ? "true" : "false");
	} else if (m_yesNoRB->isChecked()) {
		m_boolResult->setText(res ? "Yes" : "No");
	}
	else {
		RAND_LOGE("No boolean type selected");
	}
}

void Randomizer::slotRunText(void) {
	std::vector<char> lst;

	if (m_symbolsCB->isChecked()) {
		std::string txt = m_symbolsEdit->text().toStdString();
		bool found = false;
		for (auto c : txt) {
			found = false;
			for (auto check : lst) {
				if (check == c) {
					found = true;
					break;
				}
			}
			if (!found) lst.push_back(c);
		}
	}
	if (m_upperCaseCB->isChecked()) {
		for (char c = 'A'; c <= 'Z'; c++) { lst.push_back(c); }
	}
	if (m_lowerCaseCB->isChecked()) {
		for (char c = 'a'; c <= 'z'; c++) { lst.push_back(c); }
	}
	if (m_numbersCB->isChecked()) {
		for (char c = '0'; c <= '9'; c++) { lst.push_back(c); }
	}

	if (lst.empty()) {
		RAND_LOGW("Please select at least one option for the text randomizer");
		return;
	}

	std::string res;
	for (int i = 0; i < m_textLength->value(); i++) {
		res.append(1, lst[rand() % lst.size()]);
	}

	m_textResult->setText(QString::fromStdString(res));
}

void Randomizer::slotRunList(void) {
	if (m_lists.empty()) {
		RAND_LOGW("Please add a list");
		return;
	}

	QString listName = m_listTabs->tabText(m_listTabs->currentIndex());
	auto it = m_lists.find(listName);
	if (it == m_lists.end()) {
		RAND_LOGE("Entry mismatch");
		return;
	}
	QStringList lst = it->second->toPlainText().split("\n", Qt::SkipEmptyParts);
	
	if (lst.isEmpty()) {
		RAND_LOGW("Please add at least one entry to the list \"" + listName + "\"");
		return;
	}

	m_listResult->setText(lst[rand() % lst.size()]);
}

void Randomizer::slotAddList(void) {
	RandomizerDialog dia(this, m_root->getViewWidget());
	if (dia.exec() == 1) {
		QPlainTextEdit* newEdit = new QPlainTextEdit;
		m_listTabs->addTab(newEdit, dia.currentName());
		m_lists.insert_or_assign(dia.currentName(), newEdit);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

RandomizerDialog::RandomizerDialog(Randomizer* _owner, QWidget* _widget)
	: ot::Dialog(_widget), m_owner(_owner)
{
	m_centralLayout = new QVBoxLayout(this);
	m_nameLayout = new QHBoxLayout;
	m_nameLabel = new QLabel("Name:");
	m_nameEdit = new QLineEdit;
	m_nameEdit->setPlaceholderText("List name");
	m_buttonLayout = new QHBoxLayout;
	m_confirmBtn = new QPushButton("Confirm");
	m_cancelBtn = new QPushButton("Cancel");

	m_buttonLayout->addStretch(1);
	m_buttonLayout->addWidget(m_confirmBtn);
	m_buttonLayout->addWidget(m_cancelBtn);

	m_nameLayout->addWidget(m_nameLabel);
	m_nameLayout->addWidget(m_nameEdit, 1);

	m_centralLayout->addLayout(m_nameLayout);
	m_centralLayout->addLayout(m_buttonLayout);
	m_centralLayout->addStretch(1);

	this->setWindowTitle("Add list entry");
	this->connect(m_confirmBtn, &QPushButton::clicked, this, &RandomizerDialog::slotConfirm);
	this->connect(m_cancelBtn, &QPushButton::clicked, this, &RandomizerDialog::slotCancel);
}

RandomizerDialog::~RandomizerDialog() {

}

QString RandomizerDialog::currentName(void) const {
	return m_nameEdit->text();
}

void RandomizerDialog::slotConfirm(void) {
	QString n = m_nameEdit->text();
	n.remove(' ');
	n.remove('\t');
	if (n.isEmpty()) {
		QToolTip::showText(
			QPoint(m_nameEdit->pos().x(), m_nameEdit->pos().y() + m_nameEdit->height()),
			"Please provide a name for the list",
			m_nameEdit
		);
	}
	for (auto e : m_owner->listEntries()) {
		if (m_nameEdit->text() == e.first) {
			QToolTip::showText(
				QPoint(m_nameEdit->pos().x(), m_nameEdit->pos().y() + m_nameEdit->height()),
				"A list with the name \"" + m_nameEdit->text() + "\" already exists",
				m_nameEdit
			);
		}
	}
	this->accept();
}

void RandomizerDialog::slotCancel(void) {
	this->reject();
}
