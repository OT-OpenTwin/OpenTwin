#pragma once

#include "openTwin/UIPluginAPISharedTypes.h"
#include "OTCore/ServiceBase.h"

#include <string>
#include <qstring.h>

namespace ak { class aColorStyle; }

namespace ot {

	class AbstractUIWidgetInterface;

	class UIPLUGINAPI_EXPORT AbstractPluginCore {
	public:
		AbstractPluginCore();
		virtual ~AbstractPluginCore();

		// #############################################################################

		// Core functionallity

		void setUiInterface(ot::AbstractUIWidgetInterface * _interface) { m_uiInterface = _interface; }
		ot::AbstractUIWidgetInterface * uiInterface(void) { return m_uiInterface; }

		// #############################################################################

		// Pure virtual functions

		//! @brief Will be called after the plugin was loaded
		//! In this function the plugin should create its required UI controls and initialize the data
		virtual bool initialize(void) = 0;

		//! @brief Will be called if a message for this plugin was received
		//! @param _action The action to be performed
		//! @param _message Message containing information
		virtual bool dispatchAction(const std::string& _action, const std::string& _message) = 0;

		//! @brief Will be called when the appearance of the UI frontend has changed
		//! In this function all UI controls should apply the colorstyle to match the UI frontend
		virtual void setColorstye(ak::aColorStyle * _paintable) = 0;

		//! @brief Will write the provided message to the UI frontend output window
		virtual void logInfo(const std::string& _message);

		//! @brief Will write the provided message to the UI frontend output window
		virtual void logInfo(const QString& _message);

		//! @brief Will write the provided message to the UI frontend debug output window
		virtual void logDebug(const std::string& _message);

		//! @brief Will write the provided message to the UI frontend debug output window
		virtual void logDebug(const QString& _message);

	protected:
		ot::AbstractUIWidgetInterface *		m_uiInterface;

	private:

		AbstractPluginCore(AbstractPluginCore&) = delete;
		AbstractPluginCore& operator = (AbstractPluginCore&) = delete;
	};
}