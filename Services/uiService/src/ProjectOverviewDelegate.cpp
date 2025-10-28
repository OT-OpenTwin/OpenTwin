// @otlicense

// OpenTwin header
#include "ProjectOverviewHeader.h"
#include "ProjectOverviewDelegate.h"
#include "OTWidgets/TreeWidget.h"
#include "OTWidgets/ColorStyle.h"
#include "OTWidgets/GlobalColorStyle.h"

// Qt header
#include <QtGui/qpainter.h>

void ot::ProjectOverviewDelegate::paint(QPainter* _painter, const QStyleOptionViewItem& _option, const QModelIndex& _index) const {
    QStyledItemDelegate::paint(_painter, _option, _index);
}

QSize ot::ProjectOverviewDelegate::sizeHint(const QStyleOptionViewItem& _option, const QModelIndex& _index) const {
    if (!_index.data(Qt::UserRole).toBool()) {
        return QStyledItemDelegate::sizeHint(_option, _index);
    }

    QFont font = _option.font;
    font.setBold(true);
    if (font.pointSize() > 0) {
        font.setPointSize(font.pointSize() + 2);
    }
    else if (font.pixelSize() > 0) {
        font.setPixelSize(font.pixelSize() + 2);
    }
    QFontMetrics fm(font);
    return QSize(0, fm.height() + 6);
}
