//! @file LogVisualizationItemViewDialog.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "AppBase.h"
#include "Logging.h"
#include "LogVisualizationItemViewDialog.h"

// OpenTwin header
#include "OTSystem/DateTime.h"
#include "OTWidgets/Positioning.h"

// Qt header
#include <QtCore/qjsondocument.h>
#include <QtCore/qregularexpression.h>
#include <QtGui/qshortcut.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qcheckbox.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qmessagebox.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qplaintextedit.h>

LogVisualizationItemViewDialog::LogVisualizationItemViewDialog(const ot::LogMessage& _msg, size_t _index, QWidget* _parent)
	: ot::Dialog(_parent), m_msg(_msg) 
{
	// Create layouts
	QVBoxLayout* centralLayout = new QVBoxLayout;
	QGridLayout* dataLayout = new QGridLayout;
	QHBoxLayout* timeLayout = new QHBoxLayout;
	QHBoxLayout* localTimeLayout = new QHBoxLayout;
	QVBoxLayout* bigVLayout = new QVBoxLayout;
	QHBoxLayout* messageTitleLayout = new QHBoxLayout;
	QHBoxLayout* buttonLayout = new QHBoxLayout;

	// Create controls
	QLabel* timeL = new QLabel("Time (Global):");
	m_time = new QLineEdit;
	m_time->setReadOnly(true);
	m_time->setToolTip("Time where the message was received by the logger service");
	m_timeUTC = new QCheckBox("UTC");
	
	QLabel* timeLocalL = new QLabel("Time (Local):");
	m_timeLocal = new QLineEdit;
	m_timeLocal->setToolTip("Time where the message was generated in the sending service");
	m_timeLocal->setReadOnly(true);
	m_timeLocalUTC = new QCheckBox("UTC");

	QLabel* userNameL = new QLabel("User Name:");
	QLineEdit* userName = new QLineEdit(QString::fromStdString(m_msg.getUserName()));
	userName->setReadOnly(true);

	QLabel* projectNameL = new QLabel("Project Name:");
	QLineEdit* projectName = new QLineEdit(QString::fromStdString(m_msg.getProjectName()));
	projectName->setReadOnly(true);

	QLabel* senderNameL = new QLabel("Sender:");
	QLineEdit* senderName = new QLineEdit(QString::fromStdString(m_msg.getServiceName()));
	senderName->setReadOnly(true);

	QLabel* messageTypeL = new QLabel("Type:");
	QLineEdit* messageType = new QLineEdit(Logging::logMessageTypeString(m_msg));
	messageType->setReadOnly(true);

	QLabel* functionL = new QLabel("Function:");
	QLineEdit* function = new QLineEdit(QString::fromStdString(m_msg.getFunctionName()));
	function->setReadOnly(true);

	m_findMessageSyntax = new QCheckBox("Syntax check");
	m_findMessageSyntax->setToolTip("If active, a syntax check on the message will be performed.\n"
		"If for example a JSON document was found inside the message, the json document will be displayed indented.");


	QLabel* messageL = new QLabel("Message text:");
	m_message = new QPlainTextEdit(QString::fromStdString(m_msg.getText()));
	m_message->setReadOnly(true);

	m_okButton = new QPushButton("Ok");
	m_okButton->setMinimumWidth(100);
	this->connect(m_okButton, &QPushButton::clicked, this, &LogVisualizationItemViewDialog::closeCancel);

	// Create shortcuts
	m_closeShortcut = new QShortcut(QKeySequence("Esc"), this);
	this->connect(m_closeShortcut, &QShortcut::activated, this, &LogVisualizationItemViewDialog::closeCancel);

	m_recenterShortcut = new QShortcut(QKeySequence("F11"), this);
	this->connect(m_recenterShortcut, &QShortcut::activated, this, &LogVisualizationItemViewDialog::slotRecenter);

	// Setup layouts
	this->setLayout(centralLayout);
	centralLayout->addLayout(dataLayout, 0);
	centralLayout->addLayout(bigVLayout, 1);
	centralLayout->addLayout(buttonLayout, 0);

	dataLayout->addWidget(timeL, 0, 0);
	dataLayout->addLayout(timeLayout, 0, 1);
	dataLayout->addWidget(timeLocalL, 1, 0);
	dataLayout->addLayout(localTimeLayout, 1, 1);
	dataLayout->addWidget(userNameL, 2, 0);
	dataLayout->addWidget(userName, 2, 1);
	dataLayout->addWidget(projectNameL, 3, 0);
	dataLayout->addWidget(projectName, 3, 1);
	dataLayout->addWidget(senderNameL, 4, 0);
	dataLayout->addWidget(senderName, 4, 1);
	dataLayout->addWidget(messageTypeL, 5, 0);
	dataLayout->addWidget(messageType, 5, 1);
	dataLayout->addWidget(m_findMessageSyntax, 6, 1);

	timeLayout->setContentsMargins(0, 0, 0, 0);
	timeLayout->addWidget(m_time, 1);
	timeLayout->addWidget(m_timeUTC, 0);

	localTimeLayout->setContentsMargins(0, 0, 0, 0);
	localTimeLayout->addWidget(m_timeLocal, 1);
	localTimeLayout->addWidget(m_timeLocalUTC, 0);

	messageTitleLayout->addWidget(messageL, 0);
	messageTitleLayout->addStretch(1);
	messageTitleLayout->addWidget(m_findMessageSyntax, 0);

	bigVLayout->addWidget(functionL, 0);
	bigVLayout->addWidget(function, 0);
	bigVLayout->addLayout(messageTitleLayout, 0);
	bigVLayout->addWidget(m_message, 1);

	buttonLayout->addStretch(1);
	buttonLayout->addWidget(m_okButton);
	buttonLayout->addStretch(1);

	// Setup window
	this->setWindowTitle("MessageViewer (Message #" + QString::number(_index) + ") | OToolkit");
	this->setWindowFlag(Qt::WindowContextHelpButtonHint, false);
	this->setWindowIcon(AppBase::instance()->windowIcon());

	const int minWidth = 400;
	const int minHeight = 300;

	this->setMinimumSize(minWidth, minHeight);
	this->setFocusPolicy(Qt::ClickFocus);

	otoolkit::SettingsRef settings = AppBase::instance()->createSettingsInstance();
	QRect newRect(
		QPoint(settings->value("LogVisualizationItemViewDialog.X", 0).toInt(), settings->value("LogVisualizationItemViewDialog.Y", 0).toInt()),
		QSize(std::max(settings->value("LogVisualizationItemViewDialog.W", 800).toInt(), minWidth), std::max(settings->value("LogVisualizationItemViewDialog.H", 600).toInt(), minHeight))
	);
	newRect = ot::Positioning::fitOnScreen(newRect);
	this->move(newRect.topLeft());
	this->resize(newRect.size());

	m_findMessageSyntax->setChecked(settings->value("LogVisualizationItemViewDialog.FindSyntax", true).toBool());
	m_timeUTC->setChecked(settings->value("LogVisualizationItemViewDialog.TimeUTC", true).toBool());
	m_timeLocalUTC->setChecked(settings->value("LogVisualizationItemViewDialog.TimeLocalUTC", true).toBool());

	this->slotDisplayMessageText((int)m_findMessageSyntax->checkState());
	this->slotUpdateTimestamps();

	// Connect signals
	this->connect(m_findMessageSyntax, &QCheckBox::stateChanged, this, &LogVisualizationItemViewDialog::slotDisplayMessageText);
	this->connect(m_timeUTC, &QCheckBox::stateChanged, this, &LogVisualizationItemViewDialog::slotUpdateTimestamps);
	this->connect(m_timeLocalUTC, &QCheckBox::stateChanged, this, &LogVisualizationItemViewDialog::slotUpdateTimestamps);
}

LogVisualizationItemViewDialog::~LogVisualizationItemViewDialog() {

}

void LogVisualizationItemViewDialog::closeEvent(QCloseEvent* _event) {
	otoolkit::SettingsRef settings = AppBase::instance()->createSettingsInstance();
	settings->setValue("LogVisualizationItemViewDialog.X", pos().x());
	settings->setValue("LogVisualizationItemViewDialog.Y", pos().y());
	settings->setValue("LogVisualizationItemViewDialog.W", size().width());
	settings->setValue("LogVisualizationItemViewDialog.H", size().height());
	settings->setValue("LogVisualizationItemViewDialog.FindSyntax", m_findMessageSyntax->isChecked());
	settings->setValue("LogVisualizationItemViewDialog.TimeUTC", m_timeUTC->isChecked());
	settings->setValue("LogVisualizationItemViewDialog.TimeLocalUTC", m_timeLocalUTC->isChecked());

	ot::Dialog::closeEvent(_event);
}

void LogVisualizationItemViewDialog::mousePressEvent(QMouseEvent* _event) {
	if (!geometry().contains(_event->globalPos())) {
		closeCancel();
	}
	else {
		ot::Dialog::mousePressEvent(_event);
	}
}

bool LogVisualizationItemViewDialog::eventFilter(QObject* _obj, QEvent* _event) {
	if (_event->type() == QEvent::MouseButtonPress) {
		QMessageBox m(QMessageBox::Warning, "A", "B");
		m.exec();
		QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(_event);
		if (!geometry().contains(mouseEvent->globalPos())) {
			closeCancel(); // Close the dialog
			return true; // Event handled
		}
		if (mouseEvent->globalPos().x() > (this->pos().x() + this->width())) {
			closeCancel(); // Close the dialog
			return true; // Event handled
		}
	}
	return ot::Dialog::eventFilter(_obj, _event); // Pass the event to the base class
}

bool LogVisualizationItemViewDialog::event(QEvent* _event) {
	return ot::Dialog::event(_event); // Pass the event to the base class
}

void LogVisualizationItemViewDialog::slotRecenter(void) {
	this->move(0, 0);
	this->resize(800, 600);
}

void LogVisualizationItemViewDialog::slotDisplayMessageText(int _state) {
	QString str = QString::fromStdString(m_msg.getText());
	if (m_findMessageSyntax->isChecked()) {
		// JSON check
		str = this->findJsonSyntax(str);

		// Display result string
		m_message->setPlainText(str);
	}
	else {
		m_message->setPlainText(str);
	}
}

void LogVisualizationItemViewDialog::slotUpdateTimestamps() {
	if (m_timeUTC->isChecked()) {
		m_time->setText(QString::fromStdString(ot::DateTime::timestampFromMsec(m_msg.getGlobalSystemTime(), ot::DateTime::SimpleUTC)));
	}
	else {
		m_time->setText(QString::fromStdString(ot::DateTime::timestampFromMsec(m_msg.getGlobalSystemTime(), ot::DateTime::Simple)));
	}

	if (m_timeLocalUTC->isChecked()) {
		m_timeLocal->setText(QString::fromStdString(ot::DateTime::timestampFromMsec(m_msg.getLocalSystemTime(), ot::DateTime::SimpleUTC)));
	}
	else {
		m_timeLocal->setText(QString::fromStdString(ot::DateTime::timestampFromMsec(m_msg.getLocalSystemTime(), ot::DateTime::Simple)));
	}
}

QString LogVisualizationItemViewDialog::findJsonSyntax(const QString& _inputString) {
	//QRegularExpression regex("(.*?)(\\s*([\\{\\[])(?:[^{}\\[\\]]|(?3))*\\3\\s*)(.*$)");
	QRegularExpression regex("(.*?)(\\s*\\{.*\\}\\s*)(.*$)");
	QRegularExpressionMatch match = regex.match(_inputString);

	if (match.hasMatch()) {
		QString pre = match.captured(1);
		QString doc = match.captured(2);
		QString suf = match.captured(3);
		return (pre + "\n\n" +
			QJsonDocument::fromJson(QByteArray::fromStdString(doc.toStdString())).toJson(QJsonDocument::Indented) +
			"\n\n" + suf);
	}
	else {
		return _inputString; // Return an empty string if no match is found
	}
}
