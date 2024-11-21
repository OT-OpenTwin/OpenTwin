//! @file TableItemDelegate.h
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/OTAssert.h"
#include "OTWidgets/Table.h"
#include "OTWidgets/GlobalColorStyle.h"
#include "OTWidgets/TableItemDelegate.h"

// Qt header
#include <QtGui/qpainter.h>

ot::TableItemDelegate::TableItemDelegate(Table* _table)
    : QStyledItemDelegate(_table), m_table(_table)
{
    OTAssertNullptr(m_table);
    m_table->setItemDelegate(this);
}

ot::TableItemDelegate::~TableItemDelegate() {

}

void ot::TableItemDelegate::paint(QPainter* _painter, const QStyleOptionViewItem& _option, const QModelIndex& _index) const {
    // Save the painter state
    _painter->save();

    // Initialize style options
    QStyleOptionViewItem opt = _option;
    initStyleOption(&opt, _index);

    // Initialize text pen
    QPen textPen(opt.palette.color(QPalette::Text));

    // Get current color style
    const ColorStyle& style = GlobalColorStyle::instance().getCurrentStyle();

    // Find item
    QTableWidgetItem* item = m_table->item(_index.row(), _index.column());
    if (item) {
        // Check current item state
        if (opt.state & QStyle::State_MouseOver) { // Hover
            _painter->fillRect(opt.rect, style.getValue(ColorStyleValueEntry::WidgetHoverBackground).brush());
            textPen.setBrush(style.getValue(ColorStyleValueEntry::WidgetHoverForeground).brush());
        }
        else if (opt.state & QStyle::State_Selected) { // Selected
            _painter->fillRect(opt.rect, style.getValue(ColorStyleValueEntry::WidgetSelectionBackground).brush());
            textPen.setBrush(style.getValue(ColorStyleValueEntry::WidgetSelectionForeground).brush());
        }
        else if (item->background().color().isValid()) { // Item has valid color set
            _painter->fillRect(opt.rect, item->background());
        }
        else { // Default
            _painter->fillRect(opt.rect, opt.palette.brush(QPalette::Base));
        }
    }

    // Draw the item text
    QRect textRect = opt.rect.adjusted(4, 0, -4, 0);  // Adjust text rect for padding
    _painter->setPen(textPen);
    _painter->drawText(textRect, opt.displayAlignment, opt.text);

    // Draw decorations (e.g. icons) if necessary
    if (!opt.icon.isNull()) {
        QRect iconRect = opt.rect;
        iconRect.setWidth(opt.rect.height()); // Make the icon rect square
        opt.icon.paint(_painter, iconRect, Qt::AlignCenter);
    }
        
    // Restore the painter state
    _painter->restore();
}
