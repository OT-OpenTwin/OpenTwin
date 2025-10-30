// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetBase.h"
#include "OTWidgets/OTWidgetsAPIExport.h"
#include "OTWidgets/TreeWidgetItemInfo.h"

// Qt header
#include <QtCore/qlist.h>
#include <QtWidgets/qtreewidget.h>

// std header
#include <list>

namespace ot {

	class TreeItemDelegate;

	class OT_WIDGETS_API_EXPORT TreeWidget : public QTreeWidget, public ot::WidgetBase {
		Q_OBJECT
	public:
		struct ItemInformation {
			QString text;
			QIcon icon;
			NavigationTreeItem::ItemFlags flags;
		};

		TreeWidget(QWidget * _parentWidget = (QWidget*)nullptr);
		virtual ~TreeWidget();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		//! @brief Returns a pointer to the root widget of this object
		virtual QWidget* getQWidget(void) override { return this; };
		virtual const QWidget* getQWidget(void) const override { return this; };

		QTreeWidgetItem* findItem(const QString& _itemPath, char _delimiter = '/') const;
		
		//! @brief Checks if the item at the given path exists.
		//! @param _itemPath Path to the item (e.g. "Root/Child/Item" for a delimiter '/').
		//! @param _delimiter Delimiter to separate the item path.
		bool itemExists(const QString& _itemPath, char _delimiter = '/') const { return this->findItem(_itemPath, _delimiter); };

		//! @brief Checks if an item with the given text exists.
		//! The item text is the text of the single item.
		//! The item to check may be nested.
		//! @param _itemText Text to check.
		bool itemTextExists(const QString& _itemText) const;

		QString getItemPath(QTreeWidgetItem* _item, char _delimiter = '/') const;

		QTreeWidgetItem* addItem(const TreeWidgetItemInfo& _item);

		//! @brief Deselects all items.
		//! Emits itemSelectionChanged when done.
		void deselectAll(void);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Event handler

		virtual void mousePressEvent(QMouseEvent* _event) override;

		virtual void drawRow(QPainter* _painter, const QStyleOptionViewItem& _options, const QModelIndex& _index) const override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Helper

		//! @brief Checks if an item with the given text exists.
		//! The item text is the text of the single item.
		//! The item to check may be nested.
		//! @param _parent The parent item to check the childs.
		//! @param _itemText Text to check.
		bool itemTextExists(QTreeWidgetItem* _parent, const QString& _itemText) const;

		QTreeWidgetItem* findItem(QTreeWidgetItem* _item, const QStringList& _childPath) const;

		QTreeWidgetItem* findItemByText(QTreeWidgetItem* _parent, const QString& _itemText) const;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Slots

	private Q_SLOTS:
		void slotColorStyleAboutToChange(void);
		void slotColorStyleChanged(void);

	private:
		TreeItemDelegate* m_itemDeletegate;

		std::list<int> m_columnWidths;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Helper

		QTreeWidgetItem* addItem(QTreeWidgetItem* _parent, const TreeWidgetItemInfo& _item);
		void destroyRecursive(QTreeWidgetItem* _item);
		
	};

}