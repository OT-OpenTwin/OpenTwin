// @otlicense

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