// @otlicense

// OpenTwin header
#include "OTGui/Event/NavigationMenuRequestData.h"
#include "OTWidgets/Event/NavigationMenuRequestEvent.h"
#include "OTWidgets/Menu/MenuManagerHandler.h"

ot::NavigationMenuRequestEvent::NavigationMenuRequestEvent(MenuCallbackBase* _callbackBase, const QPoint& _pos)
	: MenuRequestWidgetEvent(_callbackBase, _pos)
{
	OTAssertNullptr(getTreeWidget());
}

ot::MenuRequestData* ot::NavigationMenuRequestEvent::createRequestData(const MenuManagerHandler* _handler) const
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
	bool failed = false;
	BasicEntityInformation entityInfo = _handler->getEntityInformationFromName(itemPath.toStdString(), &failed);
	if (failed)
	{
		return nullptr;
	}
	else
	{
		return new NavigationMenuRequestData(entityInfo);
	}
}

QTreeWidgetItem* ot::NavigationMenuRequestEvent::getItemAt(const QPoint& _pos) const
{
	QTreeWidget* treeWidget = this->getTreeWidget();
	OTAssertNullptr(treeWidget);
	return treeWidget->itemAt(_pos);
}
