// @otlicense
// File: SelectProjectDialog.h
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
#include "OTCore/ProjectInformation.h"
#include "OTWidgets/Dialog.h"

// Qt header
#include <QtGui/qicon.h>
#include <QtWidgets/qlistwidget.h>

// std header
#include <map>
#include <string>

namespace ot {
	class Label;
	class PushButton;
	class ProjectOverviewWidget;
}

class SelectProjectDialog : public ot::Dialog {
	Q_OBJECT
	OT_DECL_NOCOPY(SelectProjectDialog)
	OT_DECL_NOMOVE(SelectProjectDialog)
	OT_DECL_NODEFAULT(SelectProjectDialog)
public:
	SelectProjectDialog(const ot::DialogCfg& _config);
	virtual ~SelectProjectDialog();

	ot::ProjectInformation getSelectedProject() const;

private Q_SLOTS:
	void slotConfirm();
	void slotOpenRequested();
	void slotSelectionChanged();

private:
	ot::ProjectOverviewWidget* m_overview;
	ot::PushButton* m_confirmButton;
	ot::Label* m_infoLabel;
};