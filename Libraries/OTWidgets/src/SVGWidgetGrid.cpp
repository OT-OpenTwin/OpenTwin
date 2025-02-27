//! @file SVGWidgetGrid.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/SVGWidget.h"
#include "OTWidgets/SVGWidgetGrid.h"

// Qt header
#include <QtCore/qdir.h>
#include <QtGui/qevent.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qscrollarea.h>

ot::SVGWidgetGrid::SVGWidgetGrid()
	: m_itemSize(22, 22), m_rebuildTimer(this)
{
	m_rebuildInfo.widgetIx = 0;
	m_rebuildInfo.col = 0;
	m_rebuildInfo.row = 0;

	QWidget* rootW = new QWidget;
	rootW->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::MinimumExpanding);
	QHBoxLayout* hWrap = new QHBoxLayout(rootW);
	hWrap->setContentsMargins(0, 0, 0, 0);
	QVBoxLayout* vWrap = new QVBoxLayout;
	vWrap->setContentsMargins(0, 0, 0, 0);
	hWrap->addLayout(vWrap);
	hWrap->addStretch(1);

	vWrap->addLayout(m_layout = new QGridLayout);
	vWrap->addStretch(1);
	this->setWidget(rootW);
	this->setWidgetResizable(true);

	m_layout->setContentsMargins(1, 1, 1, 1);

	m_rebuildTimer.setInterval(0);
	m_rebuildTimer.setSingleShot(true);
	this->connect(&m_rebuildTimer, &QTimer::timeout, this, &SVGWidgetGrid::slotRebuildGridStep);
}

void ot::SVGWidgetGrid::fillFromPath(const QString& _rootPath) {
	QDir dir(_rootPath);
	if (!dir.exists()) {
		OT_LOG_E("Directory \"" + _rootPath.toStdString() + "\" does not exist");
		return;
	}

	this->clear();

	QStringList paths = dir.entryList(QStringList({ "*.svg" }), QDir::Files);
	m_widgets.reserve(paths.size());
	m_rebuildInfo.filePaths.reserve(m_widgets.size());
	for (QString path : paths) {
		m_rebuildInfo.filePaths.push_back(_rootPath + "/" + path);

		// Add empty widget to current widgets, it will be imported during the rebuild
		m_widgets.push_back(new SVGWidget);
		m_widgets.back()->setFixedSize(m_itemSize);
	}

	this->rebuildGrid(this->size());
}

void ot::SVGWidgetGrid::clear(void) {
	for (SVGWidget* w : m_widgets) {
		if (w) {
			w->getQWidget()->setHidden(true);
			m_layout->removeWidget(w->getQWidget());
			delete w;
		}
	}
	m_widgets.clear();
	m_rebuildInfo.filePaths.clear();
	this->resetRebuildInfo();
}

void ot::SVGWidgetGrid::setItemSize(const QSize& _size) {
	m_itemSize = _size;
	this->rebuildGrid(this->size());
}

void ot::SVGWidgetGrid::resizeEvent(QResizeEvent* _event) {
	this->rebuildGrid(_event->size());
}

void ot::SVGWidgetGrid::slotRebuildGridStep(void) {
	if (m_rebuildInfo.widgetIx >= m_widgets.size()) {
		return;
	}

	bool loaded = false;

	// Check if the widget must be reloaded
	if (!m_rebuildInfo.filePaths[m_rebuildInfo.widgetIx].isEmpty()) {
		m_widgets[m_rebuildInfo.widgetIx]->loadFromFile(m_rebuildInfo.filePaths[m_rebuildInfo.widgetIx]);
		m_rebuildInfo.filePaths[m_rebuildInfo.widgetIx].clear();
		loaded = true;
	}
	
	// Add widget
	m_widgets[m_rebuildInfo.widgetIx]->getQWidget()->setFixedSize(m_itemSize);
	m_layout->addWidget(m_widgets[m_rebuildInfo.widgetIx]->getQWidget(), m_rebuildInfo.row, m_rebuildInfo.col);
	m_widgets[m_rebuildInfo.widgetIx]->getQWidget()->setHidden(false);

	m_rebuildInfo.widgetIx++;
	m_rebuildInfo.col++;

	// Check column bounds
	if (m_rebuildInfo.col >= m_rebuildInfo.cols) {
		m_rebuildInfo.row++;
		m_rebuildInfo.col = 0;
	}

	if (loaded) {
		m_rebuildTimer.start();
	}
	else {
		this->slotRebuildGridStep();
	}
}

void ot::SVGWidgetGrid::rebuildGrid(const QSize& _newSize) {
	// Stop rebuild step timer
	m_rebuildTimer.stop();

	// Ensure there is data
	OTAssert(m_widgets.size() == m_rebuildInfo.filePaths.size(), "Data size mismatch");
	if (m_widgets.empty()) {
		return;
	}

	// Calculate new sizes
	QMargins marg = m_layout->contentsMargins();
	QSize boundSize = m_itemSize + QSize(marg.left() + marg.right(), marg.top() + marg.bottom());
	int cols = std::max(1, _newSize.width() / ((boundSize.width() + (2 * (marg.left() + marg.right())))));

	if (cols != m_rebuildInfo.cols) {
		// Remove all widgets from layout
		for (SVGWidget* w : m_widgets) {
			if (w) {
				w->getQWidget()->setHidden(true);
				m_layout->removeWidget(w->getQWidget());
			}
		}

		this->resetRebuildInfo();

		m_rebuildInfo.cols = cols;
	}

	this->slotRebuildGridStep();
}

void ot::SVGWidgetGrid::resetRebuildInfo(void) {
	m_rebuildInfo.widgetIx = 0;
	m_rebuildInfo.row = 0;
	m_rebuildInfo.col = 0;
	m_rebuildInfo.cols = 0;
}
