//! @file ProjectOverviewWidget.h
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "ControlsManager.h"
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

class ProjectOverviewEntry : public QObject {
	Q_OBJECT
	OT_DECL_NODEFAULT(ProjectOverviewEntry)
public:
	ProjectOverviewEntry(const ot::ProjectInformation& _projectInfo, const QIcon& _projectTypeIcon, bool _ownerIsCreator, QTableWidget* _table);

	void setIsChecked(bool _checked);
	bool getIsChecked() const;
	QString getProjectName() const;
	bool getOwnerIsCreator() const { return m_ownerIsCreator; };

Q_SIGNALS:
	void checkedChanged();

private Q_SLOTS:
	void slotCheckedChanged();

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

class ProjectOverviewWidget : public QObject, public ot::WidgetBase, public LockableWidget {
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

	virtual QWidget* getQWidget() override { return m_widget; };
	virtual const QWidget* getQWidget() const override { return m_widget; };

	virtual bool eventFilter(QObject* _watched, QEvent* _event) override;

	QString getCurrentProjectFilter() const;

	std::list<QString> getSelectedProjects() const;

	void refreshProjectList();
	void refreshRecentProjects();

Q_SIGNALS:
	void createProjectRequest();
	void openProjectRequest();
	void copyProjectRequest();
	void renameProjectRequest();
	void deleteProjectRequest();
	void exportProjectRequest();
	void projectAccessRequest();
	void projectOwnerRequest();

private Q_SLOTS:
	void slotUpdateItemSelection();

	void slotCreateProject();
	void slotProjectDoubleClicked(int _row, int _column);
	void slotTableHeaderItemClicked(int _column);
	void slotTableHeaderSortingChanged(int _column, Qt::SortOrder _order);

	void slotRefreshProjectList();
	void slotRefreshRecentProjects();
	void slotRefreshAllProjects();

	void slotToggleViewMode();
	void slotOpenProject();
	void slotCopyProject();
	void slotRenameProject();
	void slotDeleteProject();
	void slotExportProject();
	void slotAccessProject();
	void slotOwnerProject();

	void slotFilterChanged();
	void slotProjectCheckedChanged();

private:
	ot::ToolButton* iniToolButton(const QString& _text, const QString& _iconPath, tt::Group* _group, const QString& _toolTip);
	void clear();
	void addProject(const ot::ProjectInformation& _projectInfo, bool _ownerIsCreator);
	void updateCountLabel(bool _hasMore);
	void updateToggleViewModeButton();
	void updateToolButtonsEnabledState(bool _forceDisabled = false);
	bool hasDifferentSelectedOwner();
	ProjectOverviewEntry* findEntry(const QString& _projectName);
	
	ViewMode m_mode;

	QWidget* m_widget;

	ot::Label* m_titleLabel;

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