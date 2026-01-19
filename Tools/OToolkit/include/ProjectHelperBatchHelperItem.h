// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/TreeWidgetItem.h"

// Qt header
#include <QtCore/qobject.h>

namespace ot {
	class TreeWidget;
	class PushButton;
}

class ProjectHelperBatchHelperItem : public QObject, public ot::TreeWidgetItem {
	Q_OBJECT
	OT_DECL_NOCOPY(ProjectHelperBatchHelperItem)
	OT_DECL_NOMOVE(ProjectHelperBatchHelperItem)
public:
	enum CreateFlag : int64_t {
		NoCreateFlags  = 0 << 0,
		RemoveOTPrefix = 1 << 0
	};
	typedef ot::Flags<CreateFlag> CreateFlags;

	enum class Columns : int {
		Name,
		Edit,
		Build,
		Rebuild,
		Test,
		Clean,

		ColumnCount
	};

	static void createFromPath(ot::TreeWidget* _tree, ot::TreeWidgetItem* _parent, const QString& _path, const CreateFlags& _flags = CreateFlag::NoCreateFlags);

	ProjectHelperBatchHelperItem();
	virtual ~ProjectHelperBatchHelperItem();

	QString getScriptName() const;

private Q_SLOTS:
	void slotBuild();
	void slotRebuild();
	void slotClean();
	void slotEdit();
	void slotTest();
	void slotBuildFinished();

private:
	static ot::PushButton* createScriptButton(ot::TreeWidget* _tree, ProjectHelperBatchHelperItem* _item, int _column, const QString& _text, const QString& _path);

	QString m_rootPath;
	QString m_editPath;
	QString m_buildPath;
	QString m_testPath;
	QString m_cleanPath;
};

OT_ADD_FLAG_FUNCTIONS(ProjectHelperBatchHelperItem::CreateFlag, ProjectHelperBatchHelperItem::CreateFlags)