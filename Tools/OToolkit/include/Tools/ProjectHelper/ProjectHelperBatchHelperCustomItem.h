// @otlicense
// File: ProjectHelperBatchHelperCustomItem.h
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
#include "Tools/ProjectHelper/ProjectHelperBatchHelperItem.h"

class ScriptRunner;

namespace ot {
	class PushButton;
}

class ProjectHelperBatchHelperCustomItem : public ProjectHelperBatchHelperItem {
	Q_OBJECT
	OT_DECL_NOCOPY(ProjectHelperBatchHelperCustomItem)
	OT_DECL_NOMOVE(ProjectHelperBatchHelperCustomItem)
	OT_DECL_NODEFAULT(ProjectHelperBatchHelperCustomItem)
public:
	enum class Columns : int32_t
	{
		Run              = BaseColumns::CustomStart,
		Stop,

		ColumnCount
	};

	static void createFromPath(ot::TreeWidget* _tree, ot::TreeWidgetItem* _parent, const QString& _path, ProjectHelperBatchHelperCustomItem* _slotItem = nullptr);

	static void createFromScript(ot::TreeWidget* _tree, ot::TreeWidgetItem* _parent, const QString& _text, const QString& _rootPath, const QString& _scriptPath, const QStringList& _args = {}, bool _detached = false, ProjectHelperBatchHelperCustomItem* _slotItem = nullptr);

	virtual ~ProjectHelperBatchHelperCustomItem() = default;

	virtual void referenceDestroyed(ProjectHelperBatchHelperItem* _reference) override;

private Q_SLOTS:
	void slotRun();
	void slotStop();
	void slotRunDetached();
	void slotRunnerFinished();

protected:
	virtual ProjectHelperBatchHelperItem* createFavItemImpl(ot::TreeWidget* _tree, ot::TreeWidgetItem* _parent) override;


private:
	ProjectHelperBatchHelperCustomItem(ot::TreeWidget* _tree, ot::TreeWidgetItem* _parent, const QString& _name, const QString& _rootPath);
	static void tryCreate(ot::TreeWidget* _tree, ot::TreeWidgetItem* _parent, ProjectHelperBatchHelperCustomItem* _slotItem, const QString& _text, const QString& _rootPath, const QString& _subPath, const QStringList& _args, bool _detached);
	static ProjectHelperBatchHelperCustomItem* createItem(ot::TreeWidget* _tree, ot::TreeWidgetItem* _parent, ProjectHelperBatchHelperCustomItem* _slotItem, const QString& _text, const QString& _scriptPath, const QStringList& _args, bool _detached);
	static ot::PushButton* createScriptButton(ot::TreeWidget* _tree, ProjectHelperBatchHelperCustomItem* _item, int _column, const QString& _text, const QString& _path, bool _detached);

	void setButtonState(bool _running);

	ScriptRunner* m_runner = nullptr;
	ot::PushButton* m_runButton = nullptr;
	ot::PushButton* m_runButtonRef = nullptr;
	ot::PushButton* m_stopButton = nullptr;
	ot::PushButton* m_stopButtonRef = nullptr;
	bool m_detached = false;
	QStringList m_args;
};