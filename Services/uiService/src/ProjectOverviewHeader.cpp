// @otlicense

// OpenTwin header
#include "ProjectOverviewHeader.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/GlobalColorStyle.h"

// Qt header
#include <QtGui/qpainter.h>

ot::ProjectOverviewHeader::ProjectOverviewHeader(QWidget* _parent)
	: QHeaderView(Qt::Orientation::Horizontal, _parent), 
    c_buttonSize(16, 16), c_buttonPadding(4, 0),
    m_hoveredFilter(-1), m_pressedFilter(-1)
{
	setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	setMouseTracking(true);
	setSectionsClickable(true);
}

ot::ProjectOverviewHeader::~ProjectOverviewHeader() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Overrides

int ot::ProjectOverviewHeader::sizeHintForColumn(int _column) const {
	return QHeaderView::sizeHintForColumn(_column) + c_buttonSize.width() + c_buttonPadding.width();
}

void ot::ProjectOverviewHeader::paintSection(QPainter* _painter, const QRect& _rect, int _logicalIndex) const {
	QHeaderView::paintSection(_painter, _rect, _logicalIndex);
    return;

	const ColorStyle& cs = GlobalColorStyle::instance().getCurrentStyle();

    // Draw default section background & text
    QStyleOptionHeader opt;
    initStyleOption(&opt);
    opt.rect = _rect;
    opt.section = _logicalIndex;
    opt.text = model() ? model()->headerData(_logicalIndex, Qt::Horizontal).toString() : QString();
    opt.state |= QStyle::State_Raised;

    // Call style to draw base header
    style()->drawControl(QStyle::CE_Header, &opt, _painter, this);

    // Draw filter icon
    QRect iconRect = QRect(
        _rect.right() - c_buttonSize.width() - c_buttonPadding.width(),
        _rect.center().y() - c_buttonSize.height() / 2,
        c_buttonSize.width(),
        c_buttonSize.height()
    );

    // Choose icon based on hover/press state
    QIcon icon;
    if (m_pressedFilter == _logicalIndex) {
        icon = ot::IconManager::getIcon(cs.getFile(ColorStyleFileEntry::HeaderFilterPressedIcon));
    }
    else if (m_hoveredFilter == _logicalIndex) {
        icon = ot::IconManager::getIcon(cs.getFile(ColorStyleFileEntry::HeaderFilterHoverIcon));
    }
    else {
        icon = ot::IconManager::getIcon(cs.getFile(ColorStyleFileEntry::HeaderFilterIcon));
    }

    if (!icon.isNull()) {
        QPixmap pix = icon.pixmap(c_buttonSize);
        _painter->drawPixmap(iconRect.topLeft(), pix);
    }
}

QSize ot::ProjectOverviewHeader::sectionSizeFromContents(int _logicalIndex) const {
	QSize size = QHeaderView::sectionSizeFromContents(_logicalIndex);
	
	size = size.expandedTo(c_buttonSize + c_buttonPadding);
	size.setWidth(size.width() + c_buttonSize.width());

	return size;
}
