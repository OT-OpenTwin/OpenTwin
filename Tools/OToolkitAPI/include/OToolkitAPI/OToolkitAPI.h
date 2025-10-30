// @otlicense
// File: OToolkitAPI.h
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

