// @otlicense
// File: WindowAPI.h
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

// OpenTwin header
#include "OTGui/StyledTextBuilder.h"
#include "OTFrontendConnectorAPI/OTFrontendConnectorAPIExport.h"

// Qt header
#include <QtCore/qstring.h>

namespace ot {

	class OT_FRONTEND_CONNECTOR_API_EXPORT WindowAPI {
	public:
		static void lockUI(bool _flag = true);
		static void unlockUI() { WindowAPI::lockUI(false); };
		static void lockSelectionAndModification(bool _flag);

		//! @brief Will turn on or off the progress bar visibility and set the progress message
		static void setProgressBarVisibility(QString _progressMessage, bool _progressBaseVisible, bool _continuous);

		//! @brief Will set the percentage of the progress bar
		static void setProgressBarValue(int _progressPercentage);

		static void showInfoPrompt(const std::string& _title, const std::string& _message, const std::string& _detailedMessage = std::string());
		static void showWarningPrompt(const std::string& _title, const std::string& _message, const std::string& _detailedMessage = std::string());
		static void showErrorPrompt(const std::string& _title, const std::string& _message, const std::string& _detailedMessage = std::string());

		static void appendOutputMessage(const std::string& _message);
		static void appendOutputMessage(const StyledTextBuilder& _message);

	protected:
		virtual void lockUIAPI(bool _flag) = 0;
		virtual void lockSelectionAndModificationAPI(bool _flag) = 0;

		virtual void setProgressBarVisibilityAPI(QString _progressMessage, bool _progressBaseVisible, bool _continuous) = 0;
		virtual void setProgressBarValueAPI(int _progressPercentage) = 0;

		virtual void showInfoPromptAPI(const std::string& _title, const std::string& _message, const std::string& _detailedMessage) = 0;
		virtual void showWarningPromptAPI(const std::string& _title, const std::string& _message, const std::string& _detailedMessage) = 0;
		virtual void showErrorPromptAPI(const std::string& _title, const std::string& _message, const std::string& _detailedMessage) = 0;

		virtual void appendOutputMessageAPI(const std::string& _message) = 0;
		virtual void appendOutputMessageAPI(const StyledTextBuilder& _message) = 0;

		WindowAPI();
		virtual ~WindowAPI();

	private:
		static WindowAPI* instance();
		static WindowAPI*& getInstanceReference();

	};

}