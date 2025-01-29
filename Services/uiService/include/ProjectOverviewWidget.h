//! @file ProjectOverviewWidget.h
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "ControlsManager.h"
#include "ProjectInformation.h"
#include "OTCore/OTClassHelper.h"
#include "OTGui/GuiTypes.h"
#include "OTWidgets/QWidgetInterface.h"

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

class ProjectOverviewEntry : public QObject {
	Q_OBJECT
	OT_DECL_NODEFAULT(ProjectOverviewEntry)
public:
	ProjectOverviewEntry(const ProjectInformation& _projectInfo, const QIcon& _projectTypeIcon, bool _ownerIsCreator, QTableWidget* _table);

	void setIsChecked(bool _checked);
	bool getIsChecked(void) const;
	QString getProjectName(void) const;
	bool getOwnerIsCreator(void) const { return m_ownerIsCreator; };

Q_SIGNALS:
	void checkedChanged();

private Q_SLOTS:
	void slotCheckedChanged(void);

private:
	QTableWidget* m_table;
	bool m_ownerIsCreator;
	ot::CheckBox* m_checkBox;
	ot::TableItem* m_typeItem;
	ot::TableItem* m_nameItem;
	ot::TableItem* m_ownerItem;
	ot::TableItem* m_groupsItem;
	ot::TableItem* m_lastAccessTimeItem;

};

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

class ProjectOverviewWidget : public QObject, public ot::QWidgetInterface, public LockableWidget {
	Q_OBJECT
	OT_DECL_NODEFAULT(ProjectOverviewWidget)
public:
	enum class ViewMode {
		ViewAll,
		ViewRecent
	};

	ProjectOverviewWidget(tt::Page* _ttbPage);
	virtual ~ProjectOverviewWidget();

	virtual void setWidgetLocked(bool _isLocked) override;

	virtual QWidget* getQWidget(void) override { return m_widget; };
	virtual const QWidget* getQWidget(void) const override { return m_widget; };

	QString getCurrentProjectFilter(void) const;

	std::list<QString> getSelectedProjects(void) const;

	void refreshProjectList(void);
	void refreshRecentProjects(void);

Q_SIGNALS:
	void createProjectRequest(void);
	void openProjectRequest(void);
	void copyProjectRequest(void);
	void renameProjectRequest(void);
	void deleteProjectRequest(void);
	void exportProjectRequest(void);
	void projectAccessRequest(void);
	void projectOwnerRequest(void);

private Q_SLOTS:
	void slotUpdateItemSelection(void);

	void slotCreateProject(void);
	void slotProjectDoubleClicked(int _row, int _column);
	void slotTableHeaderItemClicked(int _column);
	void slotTableHeaderSortingChanged(int _column, Qt::SortOrder _order);

	void slotRefreshProjectList(void);
	void slotRefreshRecentProjects(void);
	void slotRefreshAllProjects(void);

	void slotToggleViewMode(void);
	void slotOpenProject(void);
	void slotCopyProject(void);
	void slotRenameProject(void);
	void slotDeleteProject(void);
	void slotExportProject(void);
	void slotAccessProject(void);
	void slotOwnerProject(void);

	void slotFilterChanged(void);
	void slotProjectCheckedChanged(void);

private:
	ot::ToolButton* iniToolButton(const QString& _text, const QString& _iconPath, tt::Group* _group, const QString& _toolTip);
	void clear(void);
	void addProject(const ProjectInformation& _projectInfo, bool _ownerIsCreator);
	void updateCountLabel(bool _hasMore);
	void updateToggleViewModeButton(void);
	void updateToolButtonsEnabledState(bool _forceDisabled = false);
	bool hasDifferentSelectedOwner(void);
	ProjectOverviewEntry* findEntry(const QString& _projectName);
	
	ViewMode m_mode;

	QWidget* m_widget;

	ot::LineEdit* m_filter;
	ot::Table* m_table;
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

	std::list<ProjectOverviewEntry*> m_entries;
};