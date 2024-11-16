//! @file TableItemDelegate.h
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/OTAssert.h"
#include "OTWidgets/Table.h"
#include "OTWidgets/TableItemDelegate.h"

ot::TableItemDelegate::TableItemDelegate(Table* _table)
    : QStyledItemDelegate(_table), m_table(_table)
{
    OTAssertNullptr(m_table);
    m_table->setItemDelegate(this);
}

ot::TableItemDelegate::~TableItemDelegate() {

}

void ot::TableItemDelegate::paint(QPainter* _painter, const QStyleOptionViewItem& _option, const QModelIndex& _index) const {
    QStyleOptionViewItem opt = _option;
    initStyleOption(&opt, _index);

    // Check if the item has a background set
    QTableWidgetItem* item = m_table->item(_index.row(), _index.column());
    if (item) {
        item->background().isDetached();
        if (item->background().color().isValid()) {
            opt.backgroundBrush = item->background();
        }
    }

    QStyledItemDelegate::paint(_painter, opt, _index);
}
