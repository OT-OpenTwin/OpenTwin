// @otlicense
// File: OnePropertyDialog.h
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
#include "OTGui/OnePropertyDialogCfg.h"
#include "OTWidgets/Dialog.h"

namespace ot {

	class PropertyInput;

	class OT_WIDGETS_API_EXPORT OnePropertyDialog : public Dialog {
		OT_DECL_NOCOPY(OnePropertyDialog)
		OT_DECL_NOMOVE(OnePropertyDialog)
	public:
		OnePropertyDialog(const OnePropertyDialogCfg& _config, QWidget* _parent = (QWidget*)nullptr);
		virtual ~OnePropertyDialog() = default;

		void addPropertyInputValueToJson(ot::JsonValue& _object, const char* _memberNameValue, ot::JsonAllocator& _allocator);
		QVariant getCurrentValue(void) const;

		bool valueHasChanged(void) const { return m_changed; };

	private Q_SLOTS:
		void slotValueChanged(void);
		void slotConfirm(void);
		
	private:
		PropertyInput* m_input;
		bool m_changed;

		OnePropertyDialog() = delete;
	};
}
