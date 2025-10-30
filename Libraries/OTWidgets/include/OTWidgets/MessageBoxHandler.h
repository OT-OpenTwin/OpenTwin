// @otlicense
// File: MessageBoxHandler.h
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
#include "OTGui/MessageDialogCfg.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT MessageBoxHandler {
	public:
		MessageBoxHandler();
		virtual ~MessageBoxHandler();

		virtual MessageDialogCfg::BasicButton showPrompt(const MessageDialogCfg& _config) = 0;
	};

}