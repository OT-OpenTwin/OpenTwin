// @otlicense

#include "TypeHelper.h"

// OpenTwin header
#include "OTWidgets/Label.h"
#include "OTWidgets/CheckBox.h"
#include "OTWidgets/ComboBox.h"
#include "OTWidgets/LineEdit.h"

// OpenTwin header
#include "OTCore/LogDispatcher.h"

// Qt header
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qgroupbox.h>
#include <QtWidgets/qscrollarea.h>

namespace intern {
	static const QString c_32Bit = "32-bit";
	static const QString c_64Bit = "64-bit";
}

bool TypeHelper::runTool(QMenu* _rootMenu, otoolkit::ToolWidgets& _content) {
	using namespace ot;

	QScrollArea* scrollArea = new QScrollArea(nullptr);
	ot::WidgetView* view = this->createCentralWidgetView(scrollArea, "Type Helper");
	_content.addView(view);

	QWidget* scrollAreaWidget = new QWidget(scrollArea);
	scrollArea->setWidgetResizable(true);
	scrollArea->setWidget(scrollAreaWidget);
	QVBoxLayout* layout = new QVBoxLayout(scrollAreaWidget);

	// Create helpers
	createNumberConversion(layout);
	
	// Finalize
	layout->addStretch(1);
	return true;
}

void TypeHelper::slotNumberConvUintToInt() {
	if (m_numberConv.uIntInput->text().isEmpty()) {
		m_numberConv.intOutput->setText("");
		return;
	}

	if (m_numberConv.uIntInputMode->currentText() == intern::c_64Bit) {
		bool ok = false;
		uint64_t value = m_numberConv.uIntInput->text().toULongLong(&ok);
		if (ok) {
			int64_t signedValue = static_cast<int64_t>(value);
			m_numberConv.intOutput->setText(QString::number(signedValue));
			m_numberConv.intOutput->setErrorForeground(false);
		}
		else {
			m_numberConv.intOutput->setText("Invalid number format");
			m_numberConv.intOutput->setErrorForeground(true);
		}
	}
	else {
		bool ok = false;
		uint32_t value = m_numberConv.uIntInput->text().toUInt(&ok);
		if (ok) {
			int32_t signedValue = static_cast<int32_t>(value);
			m_numberConv.intOutput->setText(QString::number(signedValue));
			m_numberConv.intOutput->setErrorForeground(false);
		}
		else {
			m_numberConv.intOutput->setText("Invalid number format");
			m_numberConv.intOutput->setErrorForeground(true);
		}
	}
}

void TypeHelper::slotNumberConvIntToUint() {
	if (m_numberConv.intInput->text().isEmpty()) {
		m_numberConv.uIntOutput->setText("");
		return;
	}
	if (m_numberConv.intInputMode->currentText() == intern::c_64Bit) {
		bool ok = false;
		int64_t value = m_numberConv.intInput->text().toLongLong(&ok);
		if (ok) {
			uint64_t unsignedValue = static_cast<uint64_t>(value);
			m_numberConv.uIntOutput->setText(QString::number(unsignedValue));
			m_numberConv.intInput->setErrorForeground(false);
		}
		else {
			m_numberConv.uIntOutput->setText("Invalid number format");
			m_numberConv.intInput->setErrorForeground(true);
		}
	}
	else {
		bool ok = false;
		int32_t value = m_numberConv.intInput->text().toInt(&ok);
		if (ok) {
			uint32_t unsignedValue = static_cast<uint32_t>(value);
			m_numberConv.uIntOutput->setText(QString::number(unsignedValue));
			m_numberConv.intInput->setErrorForeground(false);
		}
		else {
			m_numberConv.uIntOutput->setText("Invalid number format");
			m_numberConv.intInput->setErrorForeground(true);
		}
	}
}

void TypeHelper::slotNumverConvBaseFromDec() {
	QSignalBlocker blockerHex(m_numberConv.baseConvHex);
	QSignalBlocker blockerBin(m_numberConv.baseConvBin);

	if (m_numberConv.baseConvDec->text().isEmpty()) {
		m_numberConv.baseConvHex->setText("");
		m_numberConv.baseConvBin->setText("");
		return;
	}

	bool ok = false;
	uint64_t value = m_numberConv.baseConvDec->text().toULongLong(&ok);
	if (ok) {
		m_numberConv.baseConvDec->setErrorForeground(false);
		m_numberConv.baseConvHex->setText(tidyHex(QString::number(value, 16).toUpper()));
		m_numberConv.baseConvBin->setText(tidyBin(QString::number(value, 2)));
	}
	else {
		m_numberConv.baseConvDec->setErrorForeground(true);
		m_numberConv.baseConvHex->setText("Invalid number format");
		m_numberConv.baseConvBin->setText("Invalid number format");
	}
}

void TypeHelper::slotNumverConvBaseFromHex() {
	QSignalBlocker blockerDec(m_numberConv.baseConvDec);
	QSignalBlocker blockerBin(m_numberConv.baseConvBin);
	if (m_numberConv.baseConvHex->text().isEmpty()) {
		m_numberConv.baseConvDec->setText("");
		m_numberConv.baseConvBin->setText("");
		return;
	}

	bool ok = false;

	uint64_t value = m_numberConv.baseConvHex->text().toULongLong(&ok, 16);

	if (ok) {
		m_numberConv.baseConvHex->setErrorForeground(false);
		m_numberConv.baseConvDec->setText(QString::number(value));
		m_numberConv.baseConvBin->setText(tidyBin(QString::number(value, 2)));
	} 
	else {
		m_numberConv.baseConvHex->setErrorForeground(true);
		m_numberConv.baseConvDec->setText("Invalid number format");
		m_numberConv.baseConvBin->setText("Invalid number format");
	}
}

void TypeHelper::slotNumverConvBaseFromBin() {
	QSignalBlocker blockerDec(m_numberConv.baseConvDec);
	QSignalBlocker blockerHex(m_numberConv.baseConvHex);
	if (m_numberConv.baseConvBin->text().isEmpty()) {
		m_numberConv.baseConvDec->setText("");
		m_numberConv.baseConvHex->setText("");
		return;
	}

	bool ok = false;

	uint64_t value = m_numberConv.baseConvBin->text().toULongLong(&ok, 2);

	if (ok) {
		m_numberConv.baseConvBin->setErrorForeground(false);
		m_numberConv.baseConvDec->setText(QString::number(value));
		m_numberConv.baseConvHex->setText(tidyHex(QString::number(value, 16).toUpper()));
	}
	else {
		m_numberConv.baseConvBin->setErrorForeground(true);
		m_numberConv.baseConvDec->setText("Invalid number format");
		m_numberConv.baseConvHex->setText("Invalid number format");
	}
}

void TypeHelper::createNumberConversion(QVBoxLayout* _layout) {
	using namespace ot;

	QGroupBox* groupBox = new QGroupBox("Number Conversion");
	_layout->addWidget(groupBox);

	QVBoxLayout* mainLayout = new QVBoxLayout(groupBox);
	
	{
		QGridLayout* layout = new QGridLayout;
		mainLayout->addLayout(layout);

		int row = 0;

		// Unsigned to signed

		layout->addWidget(new Label("Unsigned:", groupBox), row, 0);

		m_numberConv.uIntInput = new LineEdit(groupBox);
		connect(m_numberConv.uIntInput, &LineEdit::textChanged, this, &TypeHelper::slotNumberConvUintToInt);
		layout->addWidget(m_numberConv.uIntInput, row, 1);

		layout->addWidget(new Label("-- to signed -->", groupBox), row, 2);

		m_numberConv.intOutput = new LineEdit(groupBox);
		m_numberConv.intOutput->setReadOnly(true);
		layout->addWidget(m_numberConv.intOutput, row, 3);

		m_numberConv.uIntInputMode = new ComboBox(groupBox);
		m_numberConv.uIntInputMode->addItem(intern::c_32Bit);
		m_numberConv.uIntInputMode->addItem(intern::c_64Bit);
		m_numberConv.uIntInputMode->setCurrentIndex(1);
		layout->addWidget(m_numberConv.uIntInputMode, row, 4);
		connect(m_numberConv.uIntInputMode, &ComboBox::currentIndexChanged, this, &TypeHelper::slotNumberConvUintToInt);

		row++;

		// Signed to unsigned

		layout->addWidget(new Label("Signed:", groupBox), row, 0);

		m_numberConv.intInput = new LineEdit(groupBox);
		connect(m_numberConv.intInput, &LineEdit::textChanged, this, &TypeHelper::slotNumberConvIntToUint);
		layout->addWidget(m_numberConv.intInput, row, 1);

		layout->addWidget(new Label("-- to unsigned -->", groupBox), row, 2);

		m_numberConv.uIntOutput = new LineEdit(groupBox);
		m_numberConv.uIntOutput->setReadOnly(true);
		layout->addWidget(m_numberConv.uIntOutput, row, 3);

		m_numberConv.intInputMode = new ComboBox(groupBox);
		m_numberConv.intInputMode->addItem(intern::c_32Bit);
		m_numberConv.intInputMode->addItem(intern::c_64Bit);
		m_numberConv.intInputMode->setCurrentIndex(1);
		layout->addWidget(m_numberConv.intInputMode, row, 4);

		connect(m_numberConv.intInputMode, &ComboBox::currentIndexChanged, this, &TypeHelper::slotNumberConvIntToUint);
	}

	mainLayout->addSpacing(20);

	{
		QGridLayout* layout = new QGridLayout;
		mainLayout->addLayout(layout);

		layout->addWidget(new Label("Decimal", groupBox), 0, 0, Qt::AlignCenter);
		layout->addWidget(new Label("Hexadecimal", groupBox), 0, 1, Qt::AlignCenter);
		layout->addWidget(new Label("Binary", groupBox), 0, 2, Qt::AlignCenter);

		m_numberConv.baseConvDec = new LineEdit(groupBox);
		layout->addWidget(m_numberConv.baseConvDec, 1, 0);
		connect(m_numberConv.baseConvDec, &LineEdit::textChanged, this, &TypeHelper::slotNumverConvBaseFromDec);

		m_numberConv.baseConvHex = new LineEdit(groupBox);
		layout->addWidget(m_numberConv.baseConvHex, 1, 1);
		connect(m_numberConv.baseConvHex, &LineEdit::textChanged, this, &TypeHelper::slotNumverConvBaseFromHex);

		m_numberConv.baseConvBin = new LineEdit(groupBox);
		layout->addWidget(m_numberConv.baseConvBin, 1, 2);
		connect(m_numberConv.baseConvBin, &LineEdit::textChanged, this, &TypeHelper::slotNumverConvBaseFromBin);
	}
}

QString TypeHelper::tidyHex(const QString& _input) {
	QString output = _input;

	qsizetype len = _input.length();
	if (len % 2 != 0) {
		output.prepend('0');
	}
	
	return output;
}

QString TypeHelper::tidyBin(const QString& _input) {
	QString output = _input;
	qsizetype len = _input.length();
	qsizetype rem = len % 8;
	if (rem != 0) {
		qsizetype toAdd = 8 - rem;
		for (qsizetype i = 0; i < toAdd; i++) {
			output.prepend('0');
		}
	}
	return output;
}
