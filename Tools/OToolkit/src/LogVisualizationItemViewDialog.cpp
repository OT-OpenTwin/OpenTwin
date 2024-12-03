//! @file LogVisualizationItemViewDialog.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "AppBase.h"
#include "Logging.h"
#include "LogVisualizationItemViewDialog.h"

// OpenTwin header
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
	: QDialog(_parent), m_msg(_msg) {
	// Create layouts
	m_centralLayout = new QVBoxLayout;
	m_dataLayout = new QGridLayout;
	m_bigVLayout = new QVBoxLayout;
	m_messageTitleLayout = new QHBoxLayout;
	m_buttonLayout = new QHBoxLayout;

	// Create controls
	m_timeL = new QLabel("Time (Global):");
	m_time = new QLineEdit(QString::fromStdString(m_msg.getGlobalSystemTime()));
	m_time->setReadOnly(true);

	m_timeLocalL = new QLabel("Time (Local):");
	m_timeLocal = new QLineEdit(QString::fromStdString(m_msg.getLocalSystemTime()));
	m_timeLocal->setReadOnly(true);

	m_userNameL = new QLabel("User Name:");
	m_userName = new QLineEdit(QString::fromStdString(m_msg.getUserName()));
	m_userName->setReadOnly(true);

	m_projectNameL = new QLabel("Project Name:");
	m_projectName = new QLineEdit(QString::fromStdString(m_msg.getProjectName()));
	m_projectName->setReadOnly(true);

	m_senderNameL = new QLabel("Sender:");
	m_senderName = new QLineEdit(QString::fromStdString(m_msg.getServiceName()));
	m_senderName->setReadOnly(true);

	m_messageTypeL = new QLabel("Type:");
	m_messageType = new QLineEdit(Logging::logMessageTypeString(m_msg));
	m_messageType->setReadOnly(true);

	m_functionL = new QLabel("Function:");
	m_function = new QLineEdit(QString::fromStdString(m_msg.getFunctionName()));
	m_function->setReadOnly(true);

	m_findMessageSyntax = new QCheckBox("Syntax check");
	m_findMessageSyntax->setToolTip("If active, a syntax check on the message will be performed.\n"
		"If for example a JSON document was found inside the message, the json document will be displayed indented.");


	m_messageL = new QLabel("Message text:");
	m_message = new QPlainTextEdit(QString::fromStdString(m_msg.getText()));
	m_message->setReadOnly(true);

	m_okButton = new QPushButton("Ok");
	m_okButton->setMinimumWidth(100);
	this->connect(m_okButton, &QPushButton::clicked, this, &LogVisualizationItemViewDialog::close);

	// Create shortcuts
	m_closeShortcut = new QShortcut(QKeySequence("Esc"), this);
	this->connect(m_closeShortcut, &QShortcut::activated, this, &LogVisualizationItemViewDialog::close);

	m_recenterShortcut = new QShortcut(QKeySequence("F11"), this);
	this->connect(m_recenterShortcut, &QShortcut::activated, this, &LogVisualizationItemViewDialog::slotRecenter);

	// Setup layouts
	this->setLayout(m_centralLayout);
	m_centralLayout->addLayout(m_dataLayout, 0);
	m_centralLayout->addLayout(m_bigVLayout, 1);
	m_centralLayout->addLayout(m_buttonLayout, 0);

	m_dataLayout->addWidget(m_timeL, 0, 0);
	m_dataLayout->addWidget(m_time, 0, 1);
	m_dataLayout->addWidget(m_timeLocalL, 1, 0);
	m_dataLayout->addWidget(m_timeLocal, 1, 1);
	m_dataLayout->addWidget(m_userNameL, 2, 0);
	m_dataLayout->addWidget(m_userName, 2, 1);
	m_dataLayout->addWidget(m_projectNameL, 3, 0);
	m_dataLayout->addWidget(m_projectName, 3, 1);
	m_dataLayout->addWidget(m_senderNameL, 4, 0);
	m_dataLayout->addWidget(m_senderName, 4, 1);
	m_dataLayout->addWidget(m_messageTypeL, 5, 0);
	m_dataLayout->addWidget(m_messageType, 5, 1);
	m_dataLayout->addWidget(m_findMessageSyntax, 6, 1);

	m_messageTitleLayout->addWidget(m_messageL, 0);
	m_messageTitleLayout->addStretch(1);
	m_messageTitleLayout->addWidget(m_findMessageSyntax, 0);

	m_bigVLayout->addWidget(m_functionL, 0);
	m_bigVLayout->addWidget(m_function, 0);
	m_bigVLayout->addLayout(m_messageTitleLayout, 0);
	m_bigVLayout->addWidget(m_message, 1);

	m_buttonLayout->addStretch(1);
	m_buttonLayout->addWidget(m_okButton);
	m_buttonLayout->addStretch(1);

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
	newRect = ot::fitOnScreen(newRect);
	this->move(newRect.topLeft());
	this->resize(newRect.size());

	m_findMessageSyntax->setChecked(settings->value("LogVisualizationItemViewDialog.FindSyntax", true).toBool());

	this->slotDisplayMessageText((int)m_findMessageSyntax->checkState());

	// Connect signals
	this->connect(m_findMessageSyntax, &QCheckBox::stateChanged, this, &LogVisualizationItemViewDialog::slotDisplayMessageText);
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
}

void LogVisualizationItemViewDialog::mousePressEvent(QMouseEvent* _event) {
	if (!geometry().contains(_event->globalPos())) {
		close();
	}
	else {
		QDialog::mousePressEvent(_event);
	}
}

bool LogVisualizationItemViewDialog::eventFilter(QObject* _obj, QEvent* _event) {
	if (_event->type() == QEvent::MouseButtonPress) {
		QMessageBox m(QMessageBox::Warning, "A", "B");
		m.exec();
		QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(_event);
		if (!geometry().contains(mouseEvent->globalPos())) {
			close(); // Close the dialog
			return true; // Event handled
		}
		if (mouseEvent->globalPos().x() > (this->pos().x() + this->width())) {
			close(); // Close the dialog
			return true; // Event handled
		}
	}
	return QDialog::eventFilter(_obj, _event); // Pass the event to the base class
}

bool LogVisualizationItemViewDialog::event(QEvent* _event) {
	return QDialog::event(_event); // Pass the event to the base class
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
