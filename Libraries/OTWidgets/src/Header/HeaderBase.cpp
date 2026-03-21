// @otlicense

// OpenTwin header
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/Header/HeaderBase.h"
#include "OTWidgets/Style/GlobalColorStyle.h"

// Qt header
#include <QtGui/qevent.h>
#include <QtGui/qpainter.h>

ot::HeaderBase::HeaderBase(Orientation _orientation, QWidget* _parent)
	: HeaderBase(_orientation == Orientation::Horizontal ? Qt::Horizontal : Qt::Vertical, _parent)
{
	setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	setMouseTracking(true);
	setSectionsClickable(true);
}

ot::HeaderBase::HeaderBase(Qt::Orientation _orientation, QWidget* _parent)
	: QHeaderView(_orientation, _parent),
	m_buttonSize(14, 14), m_buttonPadding(4, 4)
{

}

int ot::HeaderBase::sizeHintForColumn(int _column) const
{
	int hint = QHeaderView::sizeHintForColumn(_column);

	if (canFilter(_column))
	{
		hint += m_buttonSize.width() + m_buttonPadding.width();
	}

	return hint;
}

int ot::HeaderBase::sizeHintForRow(int _row) const
{
	int hint = QHeaderView::sizeHintForRow(_row);

	if (canFilter(_row))
	{
		hint += m_buttonSize.height() + m_buttonPadding.height();
	}

	return hint;
}

void ot::HeaderBase::paintSection(QPainter* _painter, const QRect& _rect, int _logicalIndex) const
{
	// Draw default section background & text
	QStyleOptionHeader opt;
	initStyleOption(&opt);
	opt.iconAlignment = Qt::AlignLeft | Qt::AlignVCenter;
	opt.textAlignment = Qt::AlignLeft | Qt::AlignVCenter;
	opt.rect = _rect;
	opt.section = _logicalIndex;
	opt.text = model() ? model()->headerData(_logicalIndex, Qt::Horizontal).toString() : QString();
	opt.state |= QStyle::State_Raised;

	// Call style to draw base header
	style()->drawControl(QStyle::CE_Header, &opt, _painter, this);

	if (!canFilter(_logicalIndex))
	{
		return;
	}

	const ColorStyle& cs = GlobalColorStyle::instance().getCurrentStyle();

	// Draw filter icon
	QRect iconRect = QRect(
		(_rect.right() - m_buttonSize.width()),
		_rect.center().y() - m_buttonSize.height() / 2,
		m_buttonSize.width(),
		m_buttonSize.height()
	);

	// Choose icon based on hover/press state
	QIcon icon;
	if (m_filterState.pressedFilter == _logicalIndex)
	{
		icon = QIcon(cs.getFile(ColorStyleFileEntry::HeaderFilterPressedIcon));
	}
	else if (m_filterState.hoveredFilter == _logicalIndex)
	{
		icon = QIcon(cs.getFile(ColorStyleFileEntry::HeaderFilterHoverIcon));
	}
	else if (m_filterState.activeFilter == _logicalIndex)
	{
		icon = QIcon(cs.getFile(ColorStyleFileEntry::HeaderFilterActiveIcon));
	}
	else
	{
		icon = QIcon(cs.getFile(ColorStyleFileEntry::HeaderFilterIcon));
	}

	if (!icon.isNull())
	{
		QPixmap pix = icon.pixmap(m_buttonSize);
		_painter->drawPixmap(iconRect.topLeft(), pix);
	}
}

QSize ot::HeaderBase::sectionSizeFromContents(int _logicalIndex) const
{
	QSize size = QHeaderView::sectionSizeFromContents(_logicalIndex);

	if (canFilter(_logicalIndex))
	{
		size = size.expandedTo(m_buttonSize + m_buttonPadding);
		size.setWidth(size.width() + m_buttonSize.width());
	}

	return size;
}

void ot::HeaderBase::mousePressEvent(QMouseEvent* _event)
{
	int ix = logicalIndexAt(_event->pos());
	if (ix >= 0 && canFilter(ix))
	{
		QRect iconRect = filterIconRect(ix);
		if (iconRect.contains(_event->pos()))
		{
			update();
			QHeaderView::mousePressEvent(_event);

			showFilterMenu(ix);
			return;
		}
		else
		{
			ix = -1;
		}
	}
	else
	{
		ix = -1;
	}

	if (ix == m_filterState.pressedFilter)
	{
		return;
	}

	m_filterState.pressedFilter = ix;
	update();
	QHeaderView::mousePressEvent(_event);
}

void ot::HeaderBase::mouseReleaseEvent(QMouseEvent* _event)
{
	if (m_filterState.pressedFilter == -1)
	{
		return;
	}

	m_filterState.pressedFilter = -1;
	update();
	QHeaderView::mouseReleaseEvent(_event);
}

void ot::HeaderBase::mouseMoveEvent(QMouseEvent* _event)
{
	int ix = logicalIndexAt(_event->pos());
	if (ix >= 0 && canFilter(ix))
	{
		QRect iconRect = filterIconRect(ix);
		if (!iconRect.contains(_event->pos()))
		{
			ix = -1;
		}
	}
	else
	{
		ix = -1;
	}

	if (ix != m_filterState.hoveredFilter)
	{
		m_filterState.hoveredFilter = ix;
		update();
		QHeaderView::mouseMoveEvent(_event);
	}
}

void ot::HeaderBase::leaveEvent(QEvent* _event)
{
	if (m_filterState.hoveredFilter == -1 && m_filterState.pressedFilter == -1)
	{
		return;
	}

	m_filterState.hoveredFilter = -1;
	m_filterState.pressedFilter = -1;
	update();
	QHeaderView::leaveEvent(_event);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Slots

void ot::HeaderBase::sortOrderChangeRequest(int _logicalIndex, Qt::SortOrder _sortOrder)
{

}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

bool ot::HeaderBase::canFilter(int _logicalIndex) const
{
	return false;
}

QRect ot::HeaderBase::filterIconRect(int _logicalIndex) const
{
	const int pos = sectionViewportPosition(_logicalIndex);
	const int size = sectionSize(_logicalIndex);
	QRect sectionRect(pos, 0, size, height());

	return QRect(
		sectionRect.right() - m_buttonSize.width(),
		sectionRect.center().y() - m_buttonSize.height() / 2,
		m_buttonSize.width(),
		m_buttonSize.height()
	);
}

void ot::HeaderBase::showFilterMenu(int _logicalIndex)
{
	
}
