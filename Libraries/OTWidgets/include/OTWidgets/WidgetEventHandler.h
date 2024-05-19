//! \file WidgetEventHandler.h
//! \author Alexander Kuester (alexk95)
//! \date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// std header
#include <list>

class QEvent;

namespace ot {

	class WidgetEventNotifier;

	//! \class WidgetEventHandler
	template <class T> class WidgetEventHandler : public T {
	public:
		WidgetEventHandler();
		virtual ~WidgetEventHandler();

		virtual bool event(QEvent* _event) override;

		void addWidgetEventNotifier(WidgetEventNotifier* _notifier);
		void removeWidgetEventNotifier(WidgetEventNotifier* _notifier);
		const std::list<WidgetEventNotifier*>& widgetEventNotifier(void) const { return m_notifier; };

	private:
		std::list<WidgetEventNotifier*> m_notifier;
	};

}

#include "OTWidgets/WidgetEventHandler.hpp"