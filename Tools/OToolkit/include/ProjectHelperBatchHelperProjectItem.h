// @otlicense

#pragma once

// OpenTwin header
#include "ProjectHelperBatchHelperItem.h"

namespace ot {
	class PushButton;
}

class ProjectHelperBatchHelperProjectItem : public ProjectHelperBatchHelperItem {
	Q_OBJECT
	OT_DECL_NOCOPY(ProjectHelperBatchHelperProjectItem)
	OT_DECL_NOMOVE(ProjectHelperBatchHelperProjectItem)
	OT_DECL_NODEFAULT(ProjectHelperBatchHelperProjectItem)
public:
	enum CreateFlag : int64_t {
		NoCreateFlags  = 0 << 0,
		RemoveOTPrefix = 1 << 0
	};
	typedef ot::Flags<CreateFlag> CreateFlags;

	enum class Columns : int32_t
	{
		Edit             = BaseColumns::CustomStart,
		Build,
		Rebuild,
		Test,
		Clean,
		
		ColumnCount
	};

	static ProjectHelperBatchHelperProjectItem* createFromPath(ot::TreeWidget* _tree, ot::TreeWidgetItem* _parent, const QString& _path, const CreateFlags& _flags = CreateFlag::NoCreateFlags, ProjectHelperBatchHelperProjectItem* _slotItem = nullptr);

	virtual ~ProjectHelperBatchHelperProjectItem();

	virtual void referenceDestroyed(ProjectHelperBatchHelperItem* _reference) override;

private Q_SLOTS:
	void slotBuild();
	void slotRebuild();
	void slotClean();
	void slotEdit();
	void slotTest();
	void slotBuildFinished();

protected:
	virtual ProjectHelperBatchHelperItem* createFavItemImpl(ot::TreeWidget* _tree, ot::TreeWidgetItem* _parent) override;

private:
	ProjectHelperBatchHelperProjectItem(ot::TreeWidget* _tree, ot::TreeWidgetItem* _parent, const QString& _name, const QString& _rootPath);
	static ot::PushButton* createScriptButton(ot::TreeWidget* _tree, ProjectHelperBatchHelperProjectItem* _item, int _column, const QString& _text, const QString& _path);

	CreateFlags m_createFlags = CreateFlag::NoCreateFlags;
	QString m_editPath;
	QString m_buildPath;
	QString m_testPath;
	QString m_cleanPath;
};

OT_ADD_FLAG_FUNCTIONS(ProjectHelperBatchHelperProjectItem::CreateFlag, ProjectHelperBatchHelperProjectItem::CreateFlags)