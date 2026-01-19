// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/TreeWidgetItem.h"

// Qt header
#include <QtCore/qobject.h>

class ScriptRunner;

namespace ot {
	class TreeWidget;
	class PushButton;
}

class ProjectHelperBatchHelperCustomItem : public QObject, public ot::TreeWidgetItem {
	Q_OBJECT
	OT_DECL_NOCOPY(ProjectHelperBatchHelperCustomItem)
	OT_DECL_NOMOVE(ProjectHelperBatchHelperCustomItem)
public:
	static void createFromPath(ot::TreeWidget* _tree, ot::TreeWidgetItem* _parent, const QString& _path);

	virtual ~ProjectHelperBatchHelperCustomItem() = default;

private Q_SLOTS:
	void slotRun();
	void slotStop();
	void slotRunDetached();
	void slotRunnerFinished();

private:
	ProjectHelperBatchHelperCustomItem();
	static void tryCreate(ot::TreeWidget* _tree, ot::TreeWidgetItem* _parent, const QString& _text, const QString& _rootPath, const QString& _subPath, bool _detached);
	static ProjectHelperBatchHelperCustomItem* createItem(ot::TreeWidget* _tree, ot::TreeWidgetItem* _parent, const QString& _text, const QString& _rootPath, const QString& _scriptPath, bool _detached);
	static ot::PushButton* createScriptButton(ot::TreeWidget* _tree, ProjectHelperBatchHelperCustomItem* _item, int _column, const QString& _text, const QString& _path, bool _detached);

	ScriptRunner* m_runner = nullptr;
	ot::PushButton* m_runButton = nullptr;
	ot::PushButton* m_stopButton = nullptr;
	QString m_name;
	QString m_path;
	QString m_rootPath;
};