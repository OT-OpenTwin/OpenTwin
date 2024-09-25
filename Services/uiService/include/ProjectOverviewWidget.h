//! @file ProjectOverviewWidget.h
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/QWidgetInterface.h"

// Qt header
#include <QtCore/qobject.h>
#include <QtCore/qstring.h>

// std header
#include <vector>

class QTableWidget;
class QTableWidgetItem;

namespace ot { class Label; }
namespace ot { class Table; }
namespace ot { class CheckBox; }
namespace ot { class LineEdit; }

class ProjectOverviewEntry : public QObject {
	Q_OBJECT
	OT_DECL_NODEFAULT(ProjectOverviewEntry)
public:
	ProjectOverviewEntry(const QString& _projectName, const QString& _owner, QTableWidget* _table);

	int getRow(void) const { return m_row; };
	bool getIsChecked(void) const;
	QString getProjectName(void) const;

Q_SIGNALS:
	void checkedChanged(int _row);

private Q_SLOTS:
	void slotCheckedChanged(void);

private:
	int m_row;
	ot::CheckBox* m_checkBox;
	QTableWidgetItem* m_nameItem;
	QTableWidgetItem* m_ownerItem;

};

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

class ProjectOverviewWidget : public QObject, public ot::QWidgetInterface {
	Q_OBJECT
public:
	ProjectOverviewWidget();
	virtual ~ProjectOverviewWidget();

	virtual QWidget* getQWidget(void) override { return m_widget; };

	void lock(bool _flag);

public Q_SLOTS:
	void slotRefreshProjectList(void);
	void slotRefreshRecentProjects(void);
	void slotRefreshAllProjects(void);
	void slotFilterChanged(void);
	void slotProjectCheckedChanged(int _row);

private:
	void clear(void);
	void addProject(const QString& _projectName, const QString& _owner);
	void updateCountLabel(void);

	QWidget* m_widget;

	ot::LineEdit* m_filter;
	ot::Table* m_table;
	ot::Label* m_countLabel;

	int m_lockCount;

	std::vector<ProjectOverviewEntry*> m_entries;
};