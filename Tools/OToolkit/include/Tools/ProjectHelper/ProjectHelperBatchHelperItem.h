// @otlicense
// File: ProjectHelperBatchHelperItem.h
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
#include "OTWidgets/Widgets/TreeWidgetItem.h"

// Qt header
#include <QtCore/qobject.h>

class ScriptRunner;

namespace ot {
	class TreeWidget;
}

class ProjectHelperBatchHelperItem : public QObject, public ot::TreeWidgetItem {
	Q_OBJECT
	OT_DECL_NOCOPY(ProjectHelperBatchHelperItem)
	OT_DECL_NOMOVE(ProjectHelperBatchHelperItem)
	OT_DECL_NODEFAULT(ProjectHelperBatchHelperItem)
public:
	enum class BaseColumns : int32_t
	{
		Name,
		RevealInExplorer,
		CustomStart
	};

	ProjectHelperBatchHelperItem(ot::TreeWidget* _tree, ot::TreeWidgetItem* _parent, const QString& _name, const QString& _rootPath);
	virtual ~ProjectHelperBatchHelperItem() = default;

	ProjectHelperBatchHelperItem* createFavItem(ot::TreeWidget* _tree, ot::TreeWidgetItem* _parent);
	virtual void referenceDestroyed(ProjectHelperBatchHelperItem* _reference) = 0;

	const QString& getItemName() const { return m_name; };
	const QString& getFullPath() const { return m_fullPath; };
	const QString& getRootPath() const { return m_rootPath; };

	bool isCopy() const { return !m_originalItemPath.isEmpty(); };
	const QString& getOriginalItemPath() const { return m_originalItemPath; };

protected Q_SLOTS:
	virtual void slotRevealInExplorer();

protected:
	static QString normalizePath(const QString& _path);

	void setOriginalItemPath(const QString& _path) { m_originalItemPath = _path; };
	virtual ProjectHelperBatchHelperItem* createFavItemImpl(ot::TreeWidget* _tree, ot::TreeWidgetItem* _parent) = 0;

private:
	QString m_name;
	QString m_rootPath;
	QString m_fullPath;
	QString m_originalItemPath;
};