// @otlicense
// File: SVGWidgetGrid.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/SVGWidget.h"
#include "OTWidgets/FlowLayout.h"
#include "OTWidgets/SVGWidgetGrid.h"

// Qt header
#include <QtCore/qdir.h>
#include <QtGui/qevent.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qscrollarea.h>

ot::SVGWidgetGrid::SVGWidgetGrid(QWidget* _parent) :
	QScrollArea(_parent), m_itemSize(22, 22), m_delayedLoadTimer(this)
{
	m_rootWidget = new QWidget(this);
	m_rootWidget->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::MinimumExpanding);
	QHBoxLayout* hWrap = new QHBoxLayout(m_rootWidget);
	hWrap->setContentsMargins(0, 0, 0, 0);
	QVBoxLayout* vWrap = new QVBoxLayout;
	vWrap->setContentsMargins(0, 0, 0, 0);
	hWrap->addLayout(vWrap);
	hWrap->addStretch(1);

	vWrap->addLayout(m_layout = new FlowLayout);
	vWrap->addStretch(1);
	this->setWidget(m_rootWidget);
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
		m_widgets.push_back(new SVGWidget(this));
		m_layout->addWidget(m_widgets.back()->getQWidget());
	}
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
	for (SVGWidget* w : m_widgets) {
		w->setFixedSize(m_itemSize);
	}
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
