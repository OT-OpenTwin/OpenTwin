// @otlicense
// File: ProjectHelperBatchHelper.h
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
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/Widgets/WidgetBase.h"
#include "Tools/ProjectHelper/ProjectHelperBatchHelperProjectItem.h"

// Qt header
#include <QtCore/qobject.h>
#include <QtCore/qsettings.h>
#include <QtCore/qjsonarray.h>

class QTreeWidgetItem;

namespace ot {
	class LineEdit;
	class TreeWidgetItem;
	class TreeWidgetFilter;
}

class ProjectHelperBatchHelper : public QObject, public ot::WidgetBase {
	Q_OBJECT
	OT_DECL_NOCOPY(ProjectHelperBatchHelper)
	OT_DECL_NOMOVE(ProjectHelperBatchHelper)
public:
	ProjectHelperBatchHelper();
	virtual ~ProjectHelperBatchHelper();

	virtual QWidget* getQWidget() override { return m_rootWidget; };
	virtual const QWidget* getQWidget() const override { return m_rootWidget; };

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

	void setRootPath(const QString& _path);
	QString getRootPath() const;

	void restoreState(QSettings& _settings);
	bool saveState(QSettings& _settings);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Public: Slots

public Q_SLOTS:
	void clear();
	void slotRefreshData();
	void slotItemChanged(QTreeWidgetItem* _item, int _column);
	void slotItemDoubleClicked(QTreeWidgetItem* _item, int _column);

private:

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Data helper

	void refreshData();
	void restoreTreeData(const QSettings& _settings);
	void restoreTreeData(const QByteArray& _data);
	void saveTreeData(QSettings& _settings) const;
	QByteArray saveTreeData() const;
	void saveItemsState(QTreeWidgetItem* _parentItem, QJsonArray& _expandedItems, QJsonArray& _checkedItems) const;
	
	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Refresh

	void refreshFavourites();
	void refreshFavourites(QTreeWidgetItem* _parent);
	ot::TreeWidgetItem* createFavouritesContainer(const QString& _containerName);

	void sortAll(QTreeWidgetItem* _parent);
	void refreshProjectsDir(const QString& _rootPath, const QIcon& _icon, const QString& _childName, const ProjectHelperBatchHelperProjectItem::CreateFlags& _flags = ProjectHelperBatchHelperProjectItem::NoCreateFlags);

	QWidget* m_rootWidget;
	ot::TreeWidgetFilter* m_tree;
	ot::TreeWidgetItem* m_favouritesItem;
	ot::LineEdit* m_rootPathEdit;

};