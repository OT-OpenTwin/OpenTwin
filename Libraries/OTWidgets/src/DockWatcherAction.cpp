// @otlicense
// File: DockWatcherAction.cpp
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
#include "OTSystem/OTAssert.h"
#include "OTWidgets/DockWatcherAction.h"

ot::DockWatcherAction::DockWatcherAction() : m_dock(nullptr) {
	this->setCheckable(true);
	this->connect(this, &QAction::triggered, this, &ot::DockWatcherAction::slotTriggered);
}

ot::DockWatcherAction::~DockWatcherAction() {

}

void ot::DockWatcherAction::watch(QDockWidget* _dock) {
	this->removeWatch();
	OTAssertNullptr(_dock);
	
	m_dock = _dock;
	this->setChecked(m_dock->isVisible());
	this->setText(m_dock->windowTitle());
	this->connect(m_dock, &QDockWidget::visibilityChanged, this, &ot::DockWatcherAction::slotVisibilityChanged);
}

void ot::DockWatcherAction::removeWatch(void) {
	if (m_dock) {
		this->disconnect(m_dock, &QDockWidget::visibilityChanged, this, &ot::DockWatcherAction::slotVisibilityChanged);
		m_dock = nullptr;
	}
}

void ot::DockWatcherAction::slotTriggered(bool _checked) {
	m_dock->setVisible(!m_dock->isVisible());
}

void ot::DockWatcherAction::slotVisibilityChanged(bool _vis) {
	this->setChecked(m_dock->isVisible());
}