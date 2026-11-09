// @otlicense
// File: NavigationMenuRequestEvent.cpp
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
