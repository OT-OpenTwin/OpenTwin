/*
 *	File:		aTreeWidget.h
 *	Package:	akWidgets
 *
 *  Created on: March 10, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

// OpenTwin header
#include "OTWidgets/QWidgetInterface.h"

 // AK header
#include <akCore/globalDataTypes.h>
#include <akCore/akCore.h>
#include <akWidgets/aWidget.h>

// Qt header
#include <qobject.h>						// Base class
#include <qlayout.h>						// layout for the VBoxLayout
#include <qicon.h>							// QIcon
#include <qcolor.h>							// QColor
#include <qstring.h>						// QString
#include <qtreewidget.h>					// base class

// C++ header
#include <map>
#include <vector>
#include <list>

// Forward declaration
class QTreeWidgetItem;
class QKeyEvent;
class QWidget;
class QMouseEvent;

namespace ak {

	// Forward declaration
	class aUidManager;
	class aObjectManager;
	class aSignalLinker;
	class aTreeWidget;
	class aTreeWidgetBase;
	class aTreeWidgetItem;
	class aLineEditWidget;

	class UICORE_API_EXPORT aTreeWidget : public QObject, public aWidget, public ot::QWidgetInterface {
		Q_OBJECT
	public:
		//! @brief Default constructor
		//! @param _messenger The globally used messenger
		//! @param _uidManager The globally used UID manager
		//! @param _dock The dock widget to display the tree at
		aTreeWidget();

		//! @brief Deconstructor
		virtual ~aTreeWidget();

		//! @brief Will return the widgets widget to display it
		virtual QWidget * widget(void) override;

		virtual QWidget* getQWidget(void) override;
		virtual const QWidget* getQWidget(void) const override;

		// ###########################################################################################################################################

		// Data manipulation

		//! @brief Will add a new item to the tree widget and return its UID
		//! @param _parentUid The UID of the parent item. If Id is -1, the item will be added as top level item
		UID add(UID _parentId = -1, const QString& _text = "", textAlignment _textAlignment = alignLeft, const QIcon& _icon = QIcon());

		//! @brief Will add new items to the tree according to the provided command
		//! The command consists of the root item and the childs.
		//! The command may look like this:
		//! root|child|child2
		//! In this case the delimiter will be the '|' and the tree will get a root item with the text "root" (if doesnt exist before)
		//! The root gets a child item with the text "child" and the child gets a child with the text "child2", so the tree looks like this:
		//! ->root
		//! ->->child
		//! ->->->child2
		//! The very last item will also get the alignment, colors and icon set.
		//! Will return the UID of the very last item.
		//! @param _cmd The tree command
		//! @param _delimiter The delimiter used in the command which seperates the items
		UID add(const QString& _cmd, char _delimiter = '|', textAlignment _textAlignment = alignLeft, const QIcon& _icon = QIcon());

		//! @brief Will clear all tree items, receivers will get a destroyed message for each item
		void clear(bool _emitEvent = true);

		//! @brief Will set enabled state of the provided item
		//! @param _itemId The UID of the item
		//! @param _enabled The enabled state of the item
		void setItemEnabled(
			UID							_itemId,
			bool						_enabled = true
		);

		//! @brief Will set the read only state of the tree
		//! If read only, the items can not be edited
		//! @param _readOnly The read only state to set
		void setIsReadOnly(
			bool						_readOnly = true
		);

		//! @brief Will set the selected state of the provided item.
		//! Will also set the selected state of the items childs if the selectAndDeselectChilds option is true
		//! @param _itemId The UID of the item
		//! @param _selected The selected state of the item
		void setItemSelected(
			UID							_itemId,
			bool						_selected = true
		);

		//! @brief Will set enabled state of the provided item
		//! @param _itemId The UID of the item
		//! @param _enabled The enabled state of the item
		void setItemVisible(
			UID							_itemId,
			bool						_visible
		);

		//! @brief Will set the text of the provided item
		//! @param _itemId The UID of the item
		//! @param _text The text to set
		void setItemText(
			UID							_itemId,
			const QString &				_text
		);

		//! @brief Will set the selected state of the provided item.
		//! Will not change the selected state of the childs item even if the selectAndDeselectChilds option is true
		//! @param _itemId The UID of the item
		//! @param _selected The selected state of the item
		void setSingleItemSelected(
			UID							_itemId,
			bool						_selected
		);

		//! @brief Will toggle the selected state of the provided item.
		//! Will also set the selected state of the items childs if the selectAndDeselectChilds option is true
		//! @param _itemId The UID of the item
		//! @param _selected The selected state of the item
		void toggleItemSelection(
			UID							_itemId
		);
			
		//! @brief Will deselect all items
		//! @param _emitEvent If true a selection changed signal will be emitted in case the tree had selected items.
		void deselectAllItems(bool _emitEvent);

		//! @brief Will set the enabled state of this tree
		void setEnabled(
			bool						_enabled = true
		);

		//! @brief Will set the visible state of this tree
		void setVisible(
			bool						_visible = true
		);

		//! @brief Will set the item icon of the specified item
		//! @param _itemId The UID of the item to set the icon at
		//! @param _icon The icon to set
		void setItemIcon(
			UID							_itemId,
			const QIcon &				_icon
		);

		//! @brief Will set the sorting enabled flag for this tree
		//! @param _enabled The enabled state to set
		void setSortingEnabled(
			bool						_enabled
		);
		
		// ###########################################################################################################################################

		// Filter

		//! @brief Will set the visible mode of the filter lineedit
		//! @param _visible If true, the filter lineedit will be visible
		void setFilterVisible(
			bool						_visible
		);

		//! @brief Will refresh the tree by means of the current filter
		//! If the filter is empty (length = 0) all items will be shown
		void applyCurrentFilter(void);

		//! @brief Will set the case sensitive mode for the filter
		//! @param _caseSensitive If true, the filter is case sensitive
		//! @param _refresh If true, the filter will be checked with the new setting
		void setFilterCaseSensitive(
			bool						_caseSensitive,
			bool						_refresh = true
		);

		//! @brief Will set the filter refresh on change mode
		//! @param _refreshOnChange If true, the filter will always be applied when he changes, othewise only on return pressed
		void setFilterRefreshOnChange(
			bool						_refreshOnChange
		);

		//! @brief Enables or disables the ability to select multiple tree items
		//! @param _multiSelection Specify whether multiple items can be selected
		void setMultiSelectionEnabled(
			bool						_multiSelection
		);

		//! @brief Enables or disables the ability to automatically select/deselect the childrens of an item
		//! @param _enabled if true, the childs of an item will be selected/deselected automatically
		void setAutoSelectAndDeselectChildrenEnabled(
			bool						_enabled
		);

		//! @brief Will expand all items in this tree
		void expandAllItems(void);

		void expandItem(
			ak::UID			_itemUID,
			bool		_expanded
		);

		bool isItemExpanded(
			ak::UID			_itemUID
		);

		//! @brief Will collapse all items in this tree
		void collapseAllItems(void);

		//! @brief Will delete the provided item from this tree
		//! @param _item The item to delete
		void deleteItem(
			UID							_itemUID,
			bool						_supressSelectionChangedEvent = false
		);

		//! @brief Will delete the provided items from this tree
		//! @param _items The items to delete
		void deleteItems(
			const std::vector<UID> &		_itemUIDs,
			bool						_supressSelectionChangedEvent = false
		);

		//! @brief Will set the items are editable flag
		//! @param _editable If true, the items can be modified by the user
		//! @param _applyToAll If true, then the new state will be applied to all existing items, otherwise this chane will only affect items created after this point
		void setItemsAreEditable(
			bool						_editable = true,
			bool						_applyToAll = true
		);

		//! @brief Will set the editable flag of one item
		//! @param _item The item id
		//! @param _editable If true, the item can be modified by the user
		void setItemIsEditable(
			UID							_itemUID,
			bool						_editable
		);

		//! @brief Will set the editable flag of the provided items item
		//! @param _item The item id
		//! @param _editable If true, the item can be modified by the user
		void setItemIsEditable(
			const std::vector<UID> &		_itemUIDs,
			bool						_editable
		);

		//! @brief Will set the select children flag of one item
		//! @param _item The item id
		//! @param _editable If true, the item will select its children when it is selected
		void setItemSelectChildren(
			UID							_itemUID,
			bool						_selectChildren
		);

		//! @brief If true all child of the matching item will be visible (not expanded)
		void setChildItemsVisibleWhenApplyingFilter(bool _visible) { m_displayChildsOnFilter = _visible; }

		// ###########################################################################################################################################

		// Information gathering

		//! @brief Returns a list of all selected items
		std::list<UID> selectedItems(void);

		//! @brief Will return all items from root to specified item as a vector where the first item is the root item
		//! @param _itemId The UID of the requested item
		std::vector<QString> getItemPath(
			UID							_itemId
		);

		//! @brief Will return all items from root to specified item as a string seperated with the provided delimiter where the first item is the root item
		//! @param _itemId The UID of the requested item
		//! @param _delimiter The delimiter between the items
		QString getItemPathString(
			UID							_itemId,
			char						_delimiter = '|'
		);

		//! @brief Will return the UID of the specified item
		//! @param _itemPath The path of the requested item
		//! @param _delimiter The delimiter of the item path
		UID getItemUID(
			const QString &				_itemPath,
			char						_delimiter = '|'
		);

		//! @brief Will return the text of the specified item
		//! @param _itemId The UID of the item to get the text of
		QString getItemText(
			UID							_itemId
		);

		//! @brief Will return the item with the specified UID
		//! @param _itemUID The UID of the item
		aTreeWidgetItem * item(UID _itemUID);

		aTreeWidgetItem* itemFromPath(const QString& _itemPath, char _delimiter = '|');

		//! @brief Will return the enabled state of this tree
		bool enabled() const;

		//! @brief Will return the count of items in this tree
		int itemCount(void) const;

		//! @brief Will return the sorting enabled state
		bool isSortingEnabled(void) const;

		bool isReadOnly(void) const { return m_isReadOnly; }

		bool getAutoSelectAndDeselectChildrenEnabled(void) { return m_selectAndDeselectChildren; }
		bool getMultiSelectionEnabled(void);

		aTreeWidgetBase * treeWidget(void) { return m_tree; }
		aLineEditWidget * filterWidget(void) { return m_filter; }

		// ###########################################################################################################################################

		// Events

		//! @brief Will send a event message to the messaging system in the name of the provided tree widget item
		//! @param _item The sender
		//! @param _eventType The type of the event
		//! @param _info1 Additional information 1
		//! @param _info2 Additional information 2
		/*void raiseItemEvent(
			UID							_itemId,
			eventType					_eventType,
			int							_info2 = 0
		);
		*/

		//! @brief Will perform the selection changed event
		//! @param _emitEvent If true a selection changed signal will be emitted
		void selectionChangedEvent(
			bool						_emitEvent = true
		);

		aTreeWidgetItem * itemAt(const QPoint& _pos);

	Q_SIGNALS:
		void keyPressed(QKeyEvent *);
		void keyReleased(QKeyEvent *);
		void cleared(void);
		void focusLost(void);
		void selectionChanged(void);
		void itemActivated(QTreeWidgetItem *, int);
		void itemChanged(QTreeWidgetItem *, int);
		void itemClicked(QTreeWidgetItem *, int);
		void itemCollapsed(QTreeWidgetItem *);
		void itemDoubleClicked(QTreeWidgetItem *, int);
		void itemFocused(QTreeWidgetItem *);
		void itemExpanded(QTreeWidgetItem *);
		void itemTextChanged(QTreeWidgetItem *, int);
		void itemLocationChanged(QTreeWidgetItem *, int);
		void itemsMoved(const QList<UID>& _itemIds, const QList<UID>& _oldParentIds, const QList<UID>& _newParentIds);
		void customContextMenuRequested(const QPoint& _pos);

	public Q_SLOTS:

		void slotTreeKeyPressed(QKeyEvent * _event);
		void slotTreeKeyReleased(QKeyEvent * _event);
		void slotTreeItemActivated(QTreeWidgetItem * _item, int _column);
		void slotTreeItemChanged(QTreeWidgetItem * _item, int _column);
		void slotTreeItemClicked(QTreeWidgetItem * _item, int _column);
		void slotTreeItemCollapsed(QTreeWidgetItem * _item);
		void slotTreeItemDoubleClicked(QTreeWidgetItem * _item, int _column);
		void slotTreeItemExpanded(QTreeWidgetItem * _item);
		void slotTreeSelectionChanged();
		void slotTreeMouseMove(QMouseEvent *);
		void slotTreeLeave(QEvent *);
		void slotCustomContextMenuRequested(const QPoint& _pos);

		//! @brief Will perform actions on the filter text changed event
		void slotFilterTextChanged(void);

		//! @brief Will perform actions on the filter enter pressed event
		void slotFilterKeyPressed(QKeyEvent * _event);

	private:

		friend class aTreeWidgetBase;

		//! @brief Create a new tree item
		//! @param _text The item to set
		//! @param _textAlignment The text alignment
		//! @param _foreColor The fore color to set
		//! @param _backColor The back color to set
		//! @param _icon The icon to set
		aTreeWidgetItem * createItem(
			const QString &					_text = "",
			textAlignment					_textAlignment = alignLeft,
			QIcon							_icon = QIcon()
		);

		void refreshAfterItemsMoved(const QList<aTreeWidgetItem *>& _items, const QList<UID>& _itemIds,
			const QList<UID>& _oldParentIds, const QList<UID>& _newParentIds);

		//! @brief Will clear the memory
		void memFree(void);

		//! @brief Will delete the childs of this item and remove them from the map
		//! @param _itm The item to clear the childs from
		void clearItem(
			aTreeWidgetItem *				_itm
		);

		aTreeWidgetBase *							m_tree;					//! The tree widget this class is managing
		aLineEditWidget *							m_filter;					//! The filter used in this tree
		QWidget *									m_widget;
		QVBoxLayout *								m_layout;					//! The layout of the tree
		bool										m_filterCaseSensitive;		//! If true, the filter is case sensitive
		bool										m_filterRefreshOnChange;	//! If true, the filter will always be applied when the text changes
		bool										m_ignoreEvents;
		bool										m_isReadOnly;
		bool										m_displayChildsOnFilter;

		UID											m_focusedItem;

		QString										m_headerText;				//! The header text of the tree

		UID											m_currentId;				//! The current id of the items

		std::map<UID, aTreeWidgetItem *>				m_items;

		bool										m_selectAndDeselectChildren;
		bool										m_itemsAreEditable;		//! If true then the items in this tree are editable
	};

	// #########################################################################################################################################

	// #########################################################################################################################################

	// #########################################################################################################################################

	//! This class represents a tree
	class UICORE_API_EXPORT aTreeWidgetBase : public QTreeWidget, public aWidget {
		Q_OBJECT
	public:
		//! @brief Constructor
		aTreeWidgetBase(aTreeWidget * _ownerTree);

		virtual ~aTreeWidgetBase();

		// #######################################################################################################
		// Event handling

		//! @brief Emits a key pressend signal a key is pressed
		virtual void keyPressEvent(QKeyEvent * _event) override;

		//! @brief Emits a key released signal a key is released
		virtual void keyReleaseEvent(QKeyEvent * _event) override;

		//! @brief Emits a mouse move event
		virtual void mouseMoveEvent(QMouseEvent *event) override;

		//! @brief Emits a FocusLeft event
		virtual void leaveEvent(QEvent *event) override;

		virtual void dropEvent(QDropEvent * _event) override;
		
		virtual void dragEnterEvent(QDragEnterEvent * _event) override;

		virtual void dragLeaveEvent(QDragLeaveEvent * _event) override;

		// #######################################################################################################

		//! @brief Will return the widgets widget to display it
		virtual QWidget * widget(void) override;

		// ####################################################################################################################################

		//! @brief Will add the provided item to the top level and store its information
		//! @param _item The item to add
		void AddTopLevelItem(
			aTreeWidgetItem *			_item
		);

		void itemWasMovedToTopLevel(aTreeWidgetItem * _item);

		//! @brief Will return the top level item with the provided text
		//! Returns nullptr if the item does not exist
		aTreeWidgetItem * topLevelItem(
			const QString &				_text
		);

		//! @brief Will return the top level item with the provided UID
		//! Returns nullptr if the item does not exist
		aTreeWidgetItem * topLevelItem(
			UID							_id
		);

		//! @brief Will return the names of all top level items
		std::vector<QString> topLevelItemsText(void);

		//! @brief Will clear the tree
		void Clear(void);

		//! @brief Will remove the topLevelItem with the provided UID
		//! Will not destroy the item
		void removeTopLevelItem(
			UID							_id
		);
		
		// ####################################################################################################################################

		//! @brief Will extract the id form the QTreeWidgetItem (which must be a treeItem)
		static UID getItemId(
			QTreeWidgetItem *				_item
		);

		QList<aTreeWidgetItem *> selectedItemsRef(void) const;
		void extendedItemSelectionInformation(const QList<aTreeWidgetItem *>& _selectedItems, QList<UID>& _selectedItemIds, QList<UID>& _itemParentIds) const;

	Q_SIGNALS:
		void keyPressed(QKeyEvent *);
		void keyReleased(QKeyEvent *);
		void mouseMove(QMouseEvent *);
		void leave(QEvent *);

	private:
		aTreeWidget *						m_ownerTree;
		std::map<UID, aTreeWidgetItem *>		m_topLevelItems;
		
		aTreeWidgetBase(aTreeWidgetBase &) = delete;
		aTreeWidgetBase& operator = (aTreeWidgetBase &) = delete;
	};

	// #########################################################################################################################################

	// #########################################################################################################################################

	// #########################################################################################################################################

	class UICORE_API_EXPORT aTreeWidgetItem : public QTreeWidgetItem, public aObject {
	public:

		//! @brief Constructor
		//! @param _newId The UID of this item
		//! @param _type The type of this item
		aTreeWidgetItem(
			UID								_newId,
			aTreeWidgetItem *				_parent = nullptr,
			int								_type = 0
		);

		//! @brief Constructor
		//! @param _view The view that creates this item
		//! @param _newId The UID of this item
		//! @param _type The type of this item
		aTreeWidgetItem(
			aTreeWidgetBase *				_view,
			UID								_newId,
			aTreeWidgetItem *				_parent = nullptr,
			int								_type = 0
		);

		//! @brief Deconstructor
		virtual ~aTreeWidgetItem();

		// ##############################################################################################

		// Setter

		//! @brief Will add a new child to this item
		//! @param _child The child to add
		void AddChild(
			aTreeWidgetItem *				_child
		);

		//! @brief Will set the provided item as the parent item
		//! @param _parent The parent item
		void setParentItem(
			aTreeWidgetItem *				_parent
		);

		//! @brief Will set the enabled state for every child of this item
		//! @param _enabled If true all childs of this item will be enabled
		void setChildsEnabled(
			bool							_enabled
		);

		//! @brief Will select/deselect the childs of this item
		//! @param _selected If true all childs of this item will be selected
		void setChildsSelected(
			bool							_selected
		);

		//! @brief Will set the visible state for every child of this item
		//! @param _visible If true all childs of this item will be visible
		void setChildsVisible(
			bool							_visible
		);

		//! @brief Will expand the most top level item that is selected and is a parent of this item. If this is the top level item, this item will be expanded
		void ensureTopLevelSelectionVisible();

		//! @brief Will expand all parent items of this item
		//! @param _expandThis If true, this item will be expanded
		void expandAllParents(
			bool							_expandThis
		);

		//! @brief Will collapse this item and all of its childs
		void collapse(void);

		void setEditable(bool _editable = true);

		void setLocked(bool _locked = true);

		//! @brief Will unhide this item if hidden
		//! @param _expandParents If true all parent objects will be expanded
		void setVisible(
			bool							_isVisible,
			bool							_expandParents = true
		);
		
		// ##############################################################################################

		// Getter

		//! @brief Will return a pointer to a top level child with the provided id.
		//! Returns nullptr if the item does not exist.
		aTreeWidgetItem * findChild(
			UID							_id
		);

		//! @brief Will return a pointer to a top level child with the provided text.
		//! Returns nullptr if the item does not exist.
		aTreeWidgetItem * findChild(
			const QString &					_text
		);

		//! @brief Will return the UID of the specified item
		//! @param _itemPath The path of the requested item
		//! @param _delimiter The delimiter of the item path
		UID getItemUID(
			const QStringList &				_itemPath,
			int								_currentIndex
		);

		//! @brief Will erase this child from the information storage
		//! This function should only be called from the child itself to clear up its data
		//! @param _id The UID of the item to erase
		void eraseChild(
			UID							_id
		);

		//! @brief Will return true if this item has childs
		bool hasChilds(void);

		//! @brief Will take and remove the first child item of this item
		aTreeWidgetItem * popFirstChild(void);

		//! @brief Will return all next level childs of this item
		const std::list<aTreeWidgetItem *> & childs(void);

		//! @brief Will return all childs of this item
		const std::list<aTreeWidgetItem *> & allChilds(void);

		//! @brief Will return the UIDs of all childs at this item
		const std::list<UID> & allChildsUIDs(void);

		//! @brief Will return the ammound of childs this item has
		int childCount(void) const;

		//! @brief Will return the id of the child
		UID id(void) const;

		//! @brief Will return the id of the parent item
		//! Returns -1 if there is no parent item
		UID parentId(void) const;

		//! @brief Will set the stored text, this value is only used to evaluate the changed event
		void setStoredText(const QString & _text) { m_text = _text; }

		//! @brief Will return the stored text
		QString storedText(void) const { return m_text; }

		//! @brief Will return all items from root to this item as a vector where the first item is the root item
		std::list<QString> getItemPath();

		//! @brief Will return all items from root to this item as a string seperated with the provided delimiter where the first item is the root item
		//! @param _itemId The UID of the requested item
		//! @param _delimiter The delimiter between the items
		QString getItemPathString(
			char							_delimiter = '|'
		);

		bool isVisible(void) const { return m_isVisible; }

		aTreeWidgetItem * parentItem(void) const { return m_parent; }

		void setSelectChilds(bool flag) { m_selectChilds = flag; }
		bool getSelectChilds(void) { return m_selectChilds; }

	private:

		void refreshEditableState(void);

		aTreeWidgetItem *				m_parent;
		std::list<aTreeWidgetItem *>	m_childs;			//! Contains all childs of this item
		std::list<aTreeWidgetItem *>	m_allChilds;
		std::list<UID>					m_allChildsUIDs;
		UID								m_id;
		QString							m_text;
		bool							m_isEditable;
		bool							m_isLockedForEdit;
		bool							m_isVisible;
		bool							m_selectChilds;

		aTreeWidgetItem() = delete;
		aTreeWidgetItem(aTreeWidgetItem &) = delete;
		aTreeWidgetItem & operator = (aTreeWidgetItem &) = delete;
	};
} // namespace ak
