// @otlicense

// OpenTwin header
#include "OTCore/Logging/Logger.h"
#include "OTWidgets/Widgets/Label.h"
#include "OTWidgets/Widgets/ProgressBar.h"
#include "OTWidgets/Widgets/DefaultStatusBar.h"

// Qt header
#include <QtWidgets/qlayout.h>

ot::DefaultStatusBar::DefaultStatusBar(QWidget* _parent)
	: StatusBar(_parent), m_stateHideTimer(this), m_progressHideTimer(this)
{
	QWidget* stateWidget = new QWidget(this);
	addWidget(stateWidget);
	m_stateLayout = new QHBoxLayout(stateWidget);
	m_stateLayout->setContentsMargins(0, 0, 0, 0);
	
	m_stateLabel = new Label(stateWidget);
	m_stateLayout->addWidget(m_stateLabel);

	QWidget* customLeftWidget = new QWidget(this);
	addWidget(customLeftWidget);
	m_customLeftLayout = new QHBoxLayout(customLeftWidget);
	m_customLeftLayout->setContentsMargins(0, 0, 0, 0);

	QWidget* spacerWidget = new QWidget(this);
	spacerWidget->setFixedHeight(28);
	addWidget(spacerWidget, 1);

	QWidget* customRightWidget = new QWidget(this);
	addWidget(customRightWidget);
	m_customRightLayout = new QHBoxLayout(customRightWidget);
	m_customRightLayout->setContentsMargins(0, 0, 0, 0);

	QWidget* progressWidget = new QWidget(this);
	addWidget(progressWidget);
	m_progressLayout = new QHBoxLayout(progressWidget);
	m_progressLayout->setContentsMargins(0, 0, 0, 0);

	m_progressLabel = new Label(progressWidget);
	m_progressLayout->addWidget(m_progressLabel);
	m_progressLabel->setHidden(true);

	m_progressBar = new ProgressBar(progressWidget);
	m_progressLayout->addWidget(m_progressBar);
	m_progressBar->setHidden(true);
	m_progressBar->setFixedWidth(180);

	QWidget* rightCornerWidget = new QWidget(this);
	addWidget(rightCornerWidget);
	m_rightCornerLayout = new QHBoxLayout(rightCornerWidget);
	m_rightCornerLayout->setContentsMargins(0, 0, 0, 0);

	// Timer
	m_stateHideTimer.setSingleShot(true);
	m_stateHideTimer.setInterval(5000);
	connect(&m_stateHideTimer, &QTimer::timeout, this, &DefaultStatusBar::hideStateMessage);

	m_progressHideTimer.setSingleShot(true);
	m_progressHideTimer.setInterval(5000);
	connect(&m_progressHideTimer, &QTimer::timeout, this, &DefaultStatusBar::hideProgress);
}

ot::DefaultStatusBar::~DefaultStatusBar()
{}

// ###########################################################################################################################################################################################################################################################################################################################

// Widget management

void ot::DefaultStatusBar::addLeftWidget(QWidget * _widget)
{
	m_customLeftLayout->addWidget(_widget);
}

void ot::DefaultStatusBar::removeLeftWidget(QWidget* _widget)
{
	m_customLeftLayout->removeWidget(_widget);
}

void ot::DefaultStatusBar::addRightWidget(QWidget* _widget)
{
	m_customRightLayout->addWidget(_widget);
}

void ot::DefaultStatusBar::removeRightWidget(QWidget * _widget)
{
	m_customRightLayout->removeWidget(_widget);
}

void ot::DefaultStatusBar::addRightCornerWidget(QWidget * _widget)
{
	m_rightCornerLayout->addWidget(_widget);
}

void ot::DefaultStatusBar::removeRightCornerWidget(QWidget * _widget)
{
	m_rightCornerLayout->removeWidget(_widget);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Public slots

void ot::DefaultStatusBar::showStateMessage(const QString& _message)
{
	m_stateHideTimer.stop();
	m_stateLabel->setText(_message);
	m_stateLabel->setHidden(false);
}

void ot::DefaultStatusBar::hideStateMessageDelayed(int _timeoutMs)
{
	m_stateHideTimer.stop();
	m_stateHideTimer.setInterval(_timeoutMs);
	m_stateHideTimer.start();
}

void ot::DefaultStatusBar::hideStateMessage()
{
	m_stateLabel->setHidden(true);
}

void ot::DefaultStatusBar::showProgress(const QString& _message, int _progress)
{
	m_progressHideTimer.stop();

	m_progressLabel->setText(_message);

	if (_progress < 0)
	{
		m_progressBar->setRange(0, 0);
	}
	else
	{
		m_progressBar->setRange(0, 100);
		m_progressBar->setValue(_progress);
	}

	m_progressLabel->setHidden(false);
	m_progressBar->setHidden(false);
}

void ot::DefaultStatusBar::hideProgressDelayed(int _timeoutMs)
{
	m_progressHideTimer.stop();

	m_progressBar->setRange(0, 100);
	m_progressBar->setValue(100);

	m_progressHideTimer.setInterval(_timeoutMs);
	m_progressHideTimer.start();
}

void ot::DefaultStatusBar::hideProgress()
{
	m_progressLabel->setHidden(true);
	m_progressBar->setHidden(true);
}

void ot::DefaultStatusBar::setProgressValue(int _progress)
{
	if (_progress < 0 || _progress > 100)
	{
		OT_LOG_ES("Progress bar value out of range: " << _progress);
	}
	else
	{
		m_progressBar->setValue(_progress);
	}
}