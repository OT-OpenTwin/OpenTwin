#pragma once

#include "openTwin/UIPluginAPISharedTypes.h"

#include <qstring.h>

#include <string>

class QWidget;

namespace ot {

	class UIPLUGINAPI_EXPORT AbstractUIWidgetInterface {
	public:
		AbstractUIWidgetInterface() {}
		virtual ~AbstractUIWidgetInterface() {}

		virtual void sendMessageToService(const std::string& _message) = 0;

		void appenInfoMessage(const char * _message);
		void appenInfoMessage(const std::string& _message);
		virtual void appenInfoMessage(const QString& _message) = 0;

		void appenDebugMessage(const char * _message);
		void appenDebugMessage(const std::string& _message);
		virtual void appenDebugMessage(const QString& _message) = 0;

		virtual bool addNewTab(const QString& _tabTitle, QWidget * _widget) = 0;

	private:

	};

}