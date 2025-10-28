// @otlicense

// OpenTwin header
#include "ProjectOverviewTree.h"
#include "ProjectOverviewDelegate.h"
#include "OTWidgets/ColorStyle.h"
#include "OTWidgets/GlobalColorStyle.h"

// Qt header
#include <QtGui/qpainter.h>
#include <QtWidgets/qheaderview.h>

ot::ProjectOverviewTree::ProjectOverviewTree(QWidget* _parent)
	: TreeWidget(_parent)
{
	setItemDelegate(new ProjectOverviewDelegate);
}

void ot::ProjectOverviewTree::drawRow(QPainter* _painter, const QStyleOptionViewItem& _option, const QModelIndex& _index) const {
    // Ensure category
    if (!_index.isValid() || !_index.data(Qt::UserRole).toBool()) {
        TreeWidget::drawRow(_painter, _option, _index);
        return;
    }

    // compute full row rect across all visible columns
    QRect fullRect = visualRect(_index);
    QHeaderView* header = this->header();
    int firstCol = header->logicalIndex(0);
    int lastCol = header->logicalIndex(header->count() - 1);

    int left = visualRect(_index.siblingAtColumn(firstCol)).left();
    int right = visualRect(_index.siblingAtColumn(lastCol)).right();
    fullRect.setLeft(left);
    fullRect.setRight(right);

    _painter->save();
    // optionally clip to viewport so we don't paint outside the visible area:
    _painter->setClipRect(viewport()->rect());

    const ColorStyle& cs = GlobalColorStyle::instance().getCurrentStyle();
    QBrush bg;
    QPen pen;
    
    bg = cs.getValue(ColorStyleValueEntry::HeaderBackground).toBrush();
    pen.setBrush(cs.getValue(ColorStyleValueEntry::HeaderForeground).toBrush());

    _painter->fillRect(fullRect, bg);

    // draw text from first column
    QString text = _index.siblingAtColumn(0).data(Qt::UserRole + 10).toString();
    _painter->setPen(pen);

    QFont f = _option.font;
	f.setBold(true);
    if (f.pointSize() > 0) {
        f.setPointSize(f.pointSize() + 2);
    }
    else if (f.pixelSize() > 0) {
        f.setPixelSize(f.pixelSize() + 2);
    }
    _painter->setFont(f);
    _painter->drawText(fullRect.adjusted(8, 0, -8, 0), Qt::AlignVCenter | Qt::AlignLeft, text);

	QRect branchRect(_option.rect);
    branchRect.setX(0);
    branchRect.setWidth(indentation());
    drawBranches(_painter, branchRect, _index);

    _painter->restore();
}
