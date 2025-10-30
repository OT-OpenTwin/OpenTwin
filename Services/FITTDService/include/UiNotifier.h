// @otlicense
// File: UiNotifier.h
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

#include "OTServiceFoundation/AbstractUiNotifier.h"

#include <string>

class UiNotifier : public ot::AbstractUiNotifier {
public:
	UiNotifier() {}
	virtual ~UiNotifier() {}

	//! @brief Will be called when a key sequence was activated in the ui
	//! @param _keySequence The key sequence that was activated
	virtual void shortcutActivated(const std::string& _keySquence) override;

private:
	UiNotifier(UiNotifier&) = delete;
	UiNotifier& operator = (UiNotifier&) = delete;
};