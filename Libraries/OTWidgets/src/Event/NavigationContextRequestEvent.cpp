// @otlicense

// OpenTwin header
#include "OTGui/Event/NavigationContextRequestData.h"
#include "OTWidgets/Event/NavigationContextRequestEvent.h"
#include "OTWidgets/ContextMenu/ContextMenuManagerHandler.h"

ot::NavigationContextRequestEvent::NavigationContextRequestEvent(ContextMenuCallbackBase* _callbackBase, const QPoint& _pos)
	: ContextRequestWidgetEvent(_callbackBase, _pos)
{
	OTAssertNullptr(getTreeWidget());
}

ot::ContextRequestData* ot::NavigationContextRequestEvent::createRequestData(const ContextMenuManagerHandler* _handler) const
{	
	QTreeWidget* treeWidget = this->getTreeWidget();
	OTAssertNullptr(treeWidget);

	const QTreeWidgetItem* item = this->getItem();

	QString itemPath;
	while (item != nullptr && item != this->getTreeWidget()->invisibleRootItem()) {
		if (!itemPath.isEmpty())
		{
			itemPath.prepend('/');
		}
		itemPath.prepend(item->text(0));
		item = item->parent();
	}

	OTAssertNullptr(_handler);
	BasicEntityInformation entityInfo = _handler->getEntityInformationFromName(itemPath.toStdString());

	return new NavigationContextRequestData(entityInfo);
}

const QTreeWidgetItem* ot::NavigationContextRequestEvent::getItemAt(const QPoint& _pos) const
{
	QTreeWidget* treeWidget = this->getTreeWidget();
	OTAssertNullptr(treeWidget);
	return treeWidget->itemAt(_pos);
}
