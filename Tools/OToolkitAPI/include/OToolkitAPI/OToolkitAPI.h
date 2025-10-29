// @otlicense

//! @file OToolkitAPI.h
//! @author Alexander Kuester (alexk95)
//! @date October 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Toolkit API header
#include "OToolkitAPI/otoolkitapi_global.h"
#include "OToolkitAPI/Tool.h"
#include "OToolkitAPI/ToolActivityNotifier.h"

// std header
#include <memory>

#define OTOOLKIT_LOG(___sender, ___message) otoolkit::api::getGlobalInterface()->log(___sender, otoolkit::APIInterface::Information, ___message);
#define OTOOLKIT_LOGW(___sender, ___message) otoolkit::api::getGlobalInterface()->log(___sender, otoolkit::APIInterface::Warning, ___message);
#define OTOOLKIT_LOGE(___sender, ___message) otoolkit::api::getGlobalInterface()->log(___sender, otoolkit::APIInterface::Error, ___message);

class QSettings;

namespace otoolkit {

	typedef std::shared_ptr<QSettings> SettingsRef;

	class OTOOLKITAPI_EXPORT APIInterface
	{
	public:
		enum InterfaceLogType {
			Information,
			Warning,
			Error
		};

		virtual void log(const QString& _sender, InterfaceLogType _type, const QString& _message) = 0;

		virtual bool addTool(Tool* _tool) = 0;

		virtual void updateStatusString(const QString& _statusText) = 0;

		virtual void updateStatusStringAsError(const QString& _statusText) = 0;

		virtual void registerToolActivityNotifier(otoolkit::ToolActivityNotifier* _notifier) = 0;

		virtual void removeToolActivityNotifier(otoolkit::ToolActivityNotifier* _notifier) = 0;

		virtual SettingsRef createSettingsInstance(void) = 0;

		virtual const QWidget* rootWidget(void) const = 0;

	protected:
		APIInterface();
		virtual ~APIInterface();
	};

	namespace api {
		OTOOLKITAPI_EXPORTONLY bool initialize(APIInterface* _interface);

		OTOOLKITAPI_EXPORTONLY otoolkit::APIInterface* getGlobalInterface(void);
	}


}

