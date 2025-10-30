// @otlicense
// File: WelcomeWidget.h
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
#include "UserManagement.h"
#include "ControlsManager.h"
#include "ProjectOverviewWidget.h"
#include "OTCore/OTClassHelper.h"
#include "OTCore/ProjectInformation.h"
#include "OTGui/GuiTypes.h"
#include "OTWidgets/WidgetBase.h"

// Qt header
#include <QtCore/qobject.h>
#include <QtCore/qstring.h>
#include <QtGui/qicon.h>

// std header
#include <map>
#include <list>
#include <string>

class QTableWidget;

namespace tt { class Page; }
namespace tt { class Group; }
namespace ot { class Label; }
namespace ot { class Table; }
namespace ot { class CheckBox; }
namespace ot { class LineEdit; }
namespace ot { class TableItem; }
namespace ot { class ToolButton; }

class WelcomeWidget : public QObject, public ot::WidgetBase, public LockableWidget {
	Q_OBJECT
	OT_DECL_NODEFAULT(WelcomeWidget)
public:
	WelcomeWidget(tt::Page* _ttbPage, UserManagement& _userManager);
	virtual ~WelcomeWidget();

	virtual void setWidgetLocked(bool _isLocked) override;

	virtual QWidget* getQWidget() override { return m_widget; };
	virtual const QWidget* getQWidget() const override { return m_widget; };

	virtual bool eventFilter(QObject* _watched, QEvent* _event) override;

	QString getGeneralFilter() const;

	std::list<ot::ProjectInformation> getSelectedProjects() const;

	void setViewMode(ot::ProjectOverviewWidget::ViewMode _mode);
	ot::ProjectOverviewWidget::ViewMode getViewMode() const { return m_overview->getViewMode(); };

	void storeViewModeSetting(UserManagement& _userManager);

Q_SIGNALS:
	void createProjectRequest();
	void openProjectRequest();
	void copyProjectRequest();
	void renameProjectRequest();
	void deleteProjectRequest();
	void exportProjectRequest();
	void projectAccessRequest();
	void projectOwnerRequest();

public Q_SLOTS:
	void slotRefreshProjectList();

private Q_SLOTS:
	void slotCreateProject();
	void slotToggleViewMode();

	void slotOpenProject();
	void slotCopyProject();
	void slotRenameProject();
	void slotDeleteProject();
	void slotExportProject();
	void slotAccessProject();
	void slotOwnerProject();

	void slotFilterChanged();
	void slotSelectionChanged();

private:
	ot::ToolButton* iniToolButton(const QString& _text, const QString& _iconPath, tt::Group* _group, const QString& _toolTip);
	void updateCountLabel();
	void updateToolButtonsEnabledState(bool _forceDisabled = false);

	QWidget* m_widget;

	ot::Label* m_titleLabel;

	ot::LineEdit* m_filter;
	ot::Label* m_countLabel;

	ot::ToolButton* m_createButton;
	ot::ToolButton* m_refreshButton;
	ot::ToolButton* m_toggleViewModeButton;
	
	ot::ToolButton* m_openButton;
	ot::ToolButton* m_copyButton;
	ot::ToolButton* m_renameButton;
	ot::ToolButton* m_deleteButton;
	ot::ToolButton* m_exportButton;
	ot::ToolButton* m_accessButton;
	ot::ToolButton* m_ownerButton;

	ot::ProjectOverviewWidget* m_overview;
};