// @otlicense

// OpenTwin header
#include "ProjectHelperBatchHelperItem.h"

ProjectHelperBatchHelperItem* ProjectHelperBatchHelperItem::createFavItem(ot::TreeWidget* _tree, ot::TreeWidgetItem* _parent) {
	ProjectHelperBatchHelperItem* itm = createFavItemImpl(_tree, _parent);
	if (itm) {
		itm->setOriginalItemPath(getTreeWidgetItemPath());
		itm->setFlags(itm->flags() & (~(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable)));
	}
	return itm;
}