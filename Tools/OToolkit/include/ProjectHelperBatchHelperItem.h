// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/TreeWidgetItem.h"

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
public:
	ProjectHelperBatchHelperItem() = default;
	virtual ~ProjectHelperBatchHelperItem() = default;

	ProjectHelperBatchHelperItem* createFavItem(ot::TreeWidget* _tree, ot::TreeWidgetItem* _parent);
	virtual void referenceDestroyed(ProjectHelperBatchHelperItem* _reference) = 0;

	bool isCopy() const { return !m_originalItemPath.isEmpty(); };
	const QString& getOriginalItemPath() const { return m_originalItemPath; };

protected:
	void setOriginalItemPath(const QString& _path) { m_originalItemPath = _path; };
	virtual ProjectHelperBatchHelperItem* createFavItemImpl(ot::TreeWidget* _tree, ot::TreeWidgetItem* _parent) = 0;

private:
	QString m_originalItemPath;
};