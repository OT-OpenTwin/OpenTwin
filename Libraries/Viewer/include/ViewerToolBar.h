// @otlicense
// File: ViewerToolBar.h
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
#include "OTCore/CoreTypes.h"

// std header
#include <list>

class ViewerToolBar {
public:
	enum ButtonType : ot::UID {
		Reset3DViewButton,
		ShowAllButton,
		ShowSelectedButton,
		HideSelectedButton,
		HideUnselectedButton,
		WireframeButton,
		WorkingPlaneButton,
		AxisCrossButton,
		CenterAxisCrossButton,
		CutplaneButton,
		TextEditorSaveButton,
		TextEditorExportButton,
		TableSaveButton,
		TableExportCSVButton,
		TableAddRowBefore,
		TableAddRowAfter,
		TableRemoveRow,
		TableAddColumnBefore,
		TabbleAddColumnAfter,
		TableRemoveColumn,

		NoButton
	};

	static ViewerToolBar& instance(void);

	ButtonType getButtonTypeFromUID(ot::UID _uid) const;

	void viewDataModifiedHasChanged(ot::WidgetViewBase::ViewType _type, bool _isModified);

	void setupDefaultControls(void);
	void setupUIControls3D(void);
	void setupUIControlsText(void);
	void setupUIControlsTable(void);
	
	void removeUIControls(void);

	void updateViewEnabledState(const ot::UIDList& _selectedTreeItems);
	void updateTextEditorEnabledState(void);
	void updateTextEditorSaveEnabledState(void);

	void updateTableEnabledState(void);
	void updateTableSaveEnabledState(void);

private:
	void resetControlsData(void);

	ViewerToolBar();
	~ViewerToolBar();

	std::list<ot::UID> m_removeItemIDList;

	ot::UID m_viewPageID;
	ot::UID m_operationsGroupID;
	ot::UID m_visiblityGroupID;
	ot::UID m_styleGroupID;
	ot::UID m_resetView3DButtonID;
	ot::UID m_showAllButtonID;
	ot::UID m_showSelectedButtonID;
	ot::UID m_hideSelectedButtonID;
	ot::UID m_hideUnselectedButtonID;
	ot::UID m_wireframeButtonID;
	ot::UID m_workingPlaneButtonID;
	ot::UID m_axisCrossButtonID;
	ot::UID m_centerAxisCrossButtonID;
	ot::UID m_cutplaneButtonID;

	ot::UID m_textEditorPageID;
	ot::UID m_textEditorDataID;
	ot::UID m_textEditorSaveID;
	ot::UID m_textEditorExportID;

	ot::UID m_tableSaveID;
	//ot::UID m_tableExportCSVID;
	ot::UID m_tableAddRowBeforeID;
	ot::UID m_tableAddRowAfterID;
	ot::UID m_tableRemoveRowID;
	ot::UID m_tableAddColumnBeforeID;
	ot::UID m_tableAddColumnAfterID;
	ot::UID m_tableRemoveColumnID;
};
