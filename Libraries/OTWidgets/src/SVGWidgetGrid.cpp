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
	: m_itemSize(22, 22)
{
	QWidget* rootW = new QWidget;
	m_layout = new QGridLayout(rootW);
	this->setWidget(rootW);
	this->setWidgetResizable(true);
}

void ot::SVGWidgetGrid::fillFromPath(const QString& _rootPath) {
	QDir dir(_rootPath);
	if (!dir.exists()) {
		OT_LOG_E("Directory \"" + _rootPath.toStdString() + "\" does not exist");
		return;
	}

	this->clear();

	QStringList paths = dir.entryList(QStringList({ "*.svg" }), QDir::Files);
	for (const QString& path : paths) {
		m_widgets.push_back(new SVGWidget(path));
		m_widgets.back()->setFixedSize(m_itemSize);
	}

	this->rebuildGrid(this->size());
}

void ot::SVGWidgetGrid::clear(void) {
	for (SVGWidget* w : m_widgets) {
		m_layout->removeWidget(w);
		delete w;
	}
	m_widgets.clear();
}

void ot::SVGWidgetGrid::resizeEvent(QResizeEvent* _event) {
	this->rebuildGrid(_event->size());
}

void ot::SVGWidgetGrid::rebuildGrid(const QSize& _newSize) {
	for (SVGWidget* w : m_widgets) {
		m_layout->removeWidget(w);
	}
	
	QMargins marg = m_layout->contentsMargins();

	QSize nextSize = m_itemSize + QSize(marg.left() + marg.right(), marg.top() + marg.bottom());
	auto it = m_widgets.begin();
	for (int r = 0; it != m_widgets.end(); r++) {
		for (int c = 0; it != m_widgets.end() && ((c + 1) * nextSize.width()) < _newSize.width(); c++, it++) {
			m_layout->addWidget(*it, r, c);
		}
		if (it == m_widgets.begin()) {
			// Not wide enough
			return;
		}
	}
}
