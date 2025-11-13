// @otlicense
// File: CommitDialog.h
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
#include "OTWidgets/Dialog.h"

namespace ot {
	
	class LineEdit;

	class CommitDialog : public Dialog {
		OT_DECL_NOCOPY(CommitDialog)
		OT_DECL_NOMOVE(CommitDialog)
		OT_DECL_NODEFAULT(CommitDialog)
	public:
		CommitDialog(QWidget* _parent);
		~CommitDialog();

	private Q_SLOTS:
		void slotConfirm();

	private:
		LineEdit* m_commitMessage;

	};
}