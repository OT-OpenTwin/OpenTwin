// @otlicense

#pragma once

// OpenTwin header
#include "ProjectHelperBatchHelperItem.h"

class ScriptRunner;

namespace ot {
	class PushButton;
}

class ProjectHelperBatchHelperCustomItem : public ProjectHelperBatchHelperItem {
	Q_OBJECT
	OT_DECL_NOCOPY(ProjectHelperBatchHelperCustomItem)
	OT_DECL_NOMOVE(ProjectHelperBatchHelperCustomItem)
public:
	static void createFromPath(ot::TreeWidget* _tree, ot::TreeWidgetItem* _parent, const QString& _path, ProjectHelperBatchHelperCustomItem* _slotItem = nullptr);

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
	ProjectHelperBatchHelperCustomItem();
	static void tryCreate(ot::TreeWidget* _tree, ot::TreeWidgetItem* _parent, ProjectHelperBatchHelperCustomItem* _slotItem, const QString& _text, const QString& _rootPath, const QString& _subPath, bool _detached);
	static ProjectHelperBatchHelperCustomItem* createItem(ot::TreeWidget* _tree, ot::TreeWidgetItem* _parent, ProjectHelperBatchHelperCustomItem* _slotItem, const QString& _text, const QString& _rootPath, const QString& _scriptPath, bool _detached);
	static ot::PushButton* createScriptButton(ot::TreeWidget* _tree, ProjectHelperBatchHelperCustomItem* _item, int _column, const QString& _text, const QString& _path, bool _detached);

	void setButtonState(bool _running);

	ScriptRunner* m_runner = nullptr;
	ot::PushButton* m_runButton = nullptr;
	ot::PushButton* m_runButtonRef = nullptr;
	ot::PushButton* m_stopButton = nullptr;
	ot::PushButton* m_stopButtonRef = nullptr;
	bool m_detached = false;
	QString m_name;
	QString m_path;
	QString m_rootPath;
};