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
	: m_itemSize(22, 22), m_delayedLoadTimer(this)
{
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

	m_loadData.ix = 0;
	
	m_delayedLoadTimer.setInterval(0);
	m_delayedLoadTimer.setSingleShot(true);
	this->connect(&m_delayedLoadTimer, &QTimer::timeout, this, &SVGWidgetGrid::slotLoadNext);
}

void ot::SVGWidgetGrid::fillFromPath(const QString& _rootPath) {
	m_delayedLoadTimer.stop();

	QDir dir(_rootPath);
	if (!dir.exists()) {
		OT_LOG_E("Directory \"" + _rootPath.toStdString() + "\" does not exist");
		return;
	}

	this->clear();

	QStringList paths = dir.entryList(QStringList({ "*.svg" }), QDir::Files);
	
	m_widgets.reserve(paths.size());
	m_loadData.paths.reserve(m_widgets.size());

	for (QString path : paths) {
		m_loadData.paths.push_back(_rootPath + "/" + path);

		// Add empty widget to current widgets, it will be imported during the rebuild
		m_widgets.push_back(new SVGWidget);
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
	m_loadData.paths.clear();
}

void ot::SVGWidgetGrid::setItemSize(const QSize& _size) {
	m_itemSize = _size;
	this->rebuildGrid(this->size());
}

void ot::SVGWidgetGrid::resizeEvent(QResizeEvent* _event) {
	this->rebuildGrid(_event->size());
}

void ot::SVGWidgetGrid::slotLoadNext(void) {
	size_t ix = m_loadData.ix;
	if (ix < m_loadData.paths.size()) {
		m_loadData.ix++;

		if (m_loadData.paths[ix].isEmpty()) {
			this->slotLoadNext();
		}
		else {
			m_widgets[ix]->loadFromFile(m_loadData.paths[ix]);
			m_loadData.paths[ix].clear();

			m_delayedLoadTimer.start();
		}
	}
}

void ot::SVGWidgetGrid::rebuildGrid(const QSize& _newSize) {
	// Stop rebuild step timer
	m_delayedLoadTimer.stop();

	// Ensure there is data
	OTAssert(m_widgets.size() == m_loadData.paths.size(), "Data size mismatch");
	if (m_widgets.empty()) {
		return;
	}

	// Calculate new sizes
	QMargins marg = m_layout->contentsMargins();
	QSize boundSize = m_itemSize + QSize(marg.left() + marg.right(), marg.top() + marg.bottom());
	int cols = std::max(1, _newSize.width() / ((boundSize.width() + (2 * (marg.left() + marg.right())))));

	int r = 0;
	int c = 0;
	for (SVGWidget* w : m_widgets) {
		w->getQWidget()->setHidden(true);
		m_layout->removeWidget(w->getQWidget());

		w->getQWidget()->setFixedSize(m_itemSize);

		m_layout->addWidget(w->getQWidget(), r, c);
		w->getQWidget()->setHidden(false);

		c++;

		// Check column bounds
		if (c >= cols) {
			r++;
			c = 0;
		}
	}

	m_loadData.ix = 0;
	m_delayedLoadTimer.start();
}
