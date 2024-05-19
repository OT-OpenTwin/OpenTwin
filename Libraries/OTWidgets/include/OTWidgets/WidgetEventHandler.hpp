//! \file WidgetEventHandler.h
//! \author Alexander Kuester (alexk95)
//! \date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetEventHandler.h"
#include "OTWidgets/WidgetEventNotifier.h"

// Qt header
#include <QtGui/qevent.h>

template <class T>
ot::WidgetEventHandler<T>::WidgetEventHandler() : m_notifier(nullptr) {}

template <class T>
ot::WidgetEventHandler<T>::~WidgetEventHandler() {}

template <class T>
bool ot::WidgetEventHandler<T>::event(QEvent* _event) {
	bool ret = T::event(_event);
	std::list<WidgetEventNotifier*> tmp = m_notifier;
	for (WidgetEventNotifier* n : tmp) {
		n->parentWidgetEvent(_event);
	}
	return ret;
}

template <class T>
void ot::WidgetEventHandler<T>::addWidgetEventNotifier(WidgetEventNotifier* _notifier) {
	_notifier->setParentWidget(this);
	m_notifier.push_back(_notifier);
}

template <class T>
void ot::WidgetEventHandler<T>::removeWidgetEventNotifier(WidgetEventNotifier* _notifier) {
	auto it = std::find(m_notifier.begin(), m_notifier.end(), _notifier);
	if (it != m_notifier.end()) {
		it->setParentWidget(nullptr);
		m_notifier.erase(it);
	}
}