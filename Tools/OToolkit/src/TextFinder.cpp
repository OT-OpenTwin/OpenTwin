#include "TextFinder.h"

// Qt header
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qlistwidget.h>

TextFinder::TextFinder() {
	// Create layouts
	m_centralLayoutW = new QWidget;
	m_centralLayout = new QVBoxLayout(m_centralLayoutW);
	m_filterLayout = new QGridLayout;
	m_resultLayout = new QHBoxLayout;
	m_buttonLayout = new QHBoxLayout;

	// Create controls
	m_buttonFind = new QPushButton("Find");

	// Setup layouts
	m_centralLayout->addLayout(m_filterLayout, 0);
	m_centralLayout->addLayout(m_resultLayout, 1);
	m_centralLayout->addLayout(m_buttonLayout, 0);

	m_buttonLayout->addStretch(1);
	m_buttonLayout->addWidget(m_buttonFind);
}

TextFinder::~TextFinder() {

}
