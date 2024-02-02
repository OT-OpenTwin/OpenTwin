//! @file SelectEntitiesDialog.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/NavigationTreeItem.h"
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
	class TreeWidgetFilter;
}

class SelectEntitiesDialog : public ot::Dialog {
	Q_OBJECT
public:
	SelectEntitiesDialog(const std::list<ot::NavigationTreeItem>& _availableItems, const std::list<ot::NavigationTreeItem>& _selectedItems, QWidget* _parent = (QWidget*)nullptr);
	~SelectEntitiesDialog();

	bool hasChanged(void) const { return m_changed; };

	//! @brief Returns the paths of the selected items
	//! @param _pathDelimiter The character used to separete the item names
	//! @param _bottomLevelOnly If true the parent items wont be added to the list
	std::list<std::string> selectedItemPaths(char _pathDelimiter = '/', bool _bottomLevelOnly = true) const;

private slots:
	void slotApply(void);
	void slotCancel(void);
	void slotAdd(QTreeWidgetItem* _item, int _col);
	void slotRemove(QTreeWidgetItem* _item, int _col);

private:
	void addSelectedPaths(QTreeWidgetItem* _item, std::list<std::string>& _list, char _pathDelimiter, bool _bottomLevelOnly) const;
	void addItem(ot::TreeWidget* _tree, QTreeWidgetItem* _parentItem, const ot::NavigationTreeItem& _item);

	bool m_changed;
	ot::TreeWidgetFilter* m_available;
	ot::TreeWidgetFilter* m_selected;
};