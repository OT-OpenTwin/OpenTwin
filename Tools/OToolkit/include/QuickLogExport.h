// @otlicense
// File: QuickLogExport.h
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
#include "OTCore/LogDispatcher.h"
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/Dialog.h"

namespace ot { class CheckBox; }
namespace ot { class ComboBox; }

class QuickLogExport : public ot::Dialog {
	Q_OBJECT
	OT_DECL_NOCOPY(QuickLogExport)
	OT_DECL_NOMOVE(QuickLogExport)
	OT_DECL_NODEFAULT(QuickLogExport)
public:
	explicit QuickLogExport(const std::list<ot::LogMessage>& _messages, QWidget* _parent);
	virtual ~QuickLogExport();

	bool isAutoClose(void);

private Q_SLOTS:
	void slotExport(void);

private:
	bool messageOkForExport(const ot::LogMessage& _message);

	std::list<ot::LogMessage> m_messages;

	ot::CheckBox* m_autoClose;
	ot::ComboBox* m_user;
	ot::ComboBox* m_project;
};