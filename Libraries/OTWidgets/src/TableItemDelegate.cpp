// @otlicense

//! @file TableItemDelegate.h
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTSystem/OTAssert.h"
#include "OTWidgets/Table.h"
#include "OTWidgets/TableTextEdit.h"
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
    m_table->setItemDelegate(nullptr);
	m_table = nullptr;
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
            _painter->fillRect(opt.rect, style.getValue(ColorStyleValueEntry::WidgetHoverBackground).toBrush());
            textPen.setBrush(style.getValue(ColorStyleValueEntry::WidgetHoverForeground).toBrush());
        } 
        else if (opt.state & QStyle::State_Selected) { // Selected
            _painter->fillRect(opt.rect, style.getValue(ColorStyleValueEntry::WidgetSelectionBackground).toBrush());
            textPen.setBrush(style.getValue(ColorStyleValueEntry::WidgetSelectionForeground).toBrush());
        }
        else {
            if (item->background().color().isValid()) { // Item has valid color set
                _painter->fillRect(opt.rect, item->background());
            }
            else { // Default
                _painter->fillRect(opt.rect, opt.palette.brush(QPalette::Base));
            }

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

QSize ot::TableItemDelegate::sizeHint(const QStyleOptionViewItem& _option, const QModelIndex& _index) const {
    return QStyledItemDelegate::sizeHint(_option, _index) + QSize(4, 4);
}

QWidget* ot::TableItemDelegate::createEditor(QWidget* _parent, const QStyleOptionViewItem& _option, const QModelIndex& _index) const {
    TableTextEdit* newEditor = new TableTextEdit(_parent, m_table, _index);
    newEditor->setContentsMargins(QMargins(2, 2, 2, 2));
    return newEditor;
}

void ot::TableItemDelegate::setEditorData(QWidget* _editor, const QModelIndex& _index) const {
    TableTextEdit* textEdit = qobject_cast<TableTextEdit*>(_editor);
    if (textEdit) {
        textEdit->setPlainText(_index.data(Qt::EditRole).toString());
        m_table->resizeColumnToContents(_index.column());
        if (m_table->getMultilineCells()) {
            m_table->resizeRowToContents(_index.row());
        }
    }
}

void ot::TableItemDelegate::updateEditorGeometry(QWidget* _editor, const QStyleOptionViewItem& _option, const QModelIndex& _index) const {
    _editor->setGeometry(_option.rect);
}
