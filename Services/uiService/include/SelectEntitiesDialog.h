// @otlicense
// File: SelectEntitiesDialog.h
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

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTGui/NavigationTreeItem.h"
#include "OTGui/NavigationTreePackage.h"
#include "OTGui/SelectEntitiesDialogCfg.h"
#include "OTWidgets/Dialog.h"

// Qt header
#include <QtCore/qstringlist.h>

// std header
#include <list>

class QVBoxLayout;
class QHBoxLayout;
class QTreeWidgetItem;

namespace ot { 
	class TreeWidget;
	class TreeWidgetItem;
	class TreeWidgetFilter;
}

class SelectEntitiesDialog : public ot::Dialog {
	Q_OBJECT
	OT_DECL_NOCOPY(SelectEntitiesDialog)
	OT_DECL_NOMOVE(SelectEntitiesDialog)
	OT_DECL_NODEFAULT(SelectEntitiesDialog)
public:
	explicit SelectEntitiesDialog(const ot::SelectEntitiesDialogCfg& _config, QWidget* _parent);
	virtual ~SelectEntitiesDialog();

	bool selectionHasChanged(void) const;

	//! @brief Returns the paths of the selected items
	//! @param _pathDelimiter The character used to separete the item names
	//! @param _bottomLevelOnly If true the parent items wont be added to the list
	std::list<std::string> selectedItemPaths(char _pathDelimiter = '/', bool _bottomLevelOnly = true) const;

private Q_SLOTS:
	void slotAdd(QTreeWidgetItem* _item, int _col);
	void slotRemove(QTreeWidgetItem* _item, int _col);

private:
	void addSelectedPaths(QTreeWidgetItem* _item, std::list<std::string>& _list, char _pathDelimiter, bool _bottomLevelOnly) const;
	ot::TreeWidgetItem* addItem(ot::TreeWidget* _tree, QTreeWidgetItem* _parentItem, const ot::NavigationTreeItem& _item);

	ot::NavigationTreePackage::NavigationTreePackageFlags m_flags;
	std::list<std::string> m_initiallySelected;
	ot::TreeWidgetFilter* m_available;
	ot::TreeWidgetFilter* m_selected;
};
