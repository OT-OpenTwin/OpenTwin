// @otlicense

// OpenTwin header
#include "OTCore/Logging/Logger.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/Header/HeaderBase.h"
#include "OTWidgets/Header/HeaderFilter.h"
#include "OTWidgets/Style/GlobalColorStyle.h"

// Qt header
#include <QtGui/qevent.h>
#include <QtGui/qpainter.h>

ot::HeaderBase::HeaderBase(Orientation _orientation, QWidget* _parent) : HeaderBase(_orientation == Orientation::Horizontal ? Qt::Horizontal : Qt::Vertical, _parent) {}

ot::HeaderBase::HeaderBase(Qt::Orientation _orientation, QWidget* _parent)
	: QHeaderView(_orientation, _parent),
	m_buttonSize(14, 14), m_buttonPadding(4, 4)
{
	setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	setMouseTracking(true);
	setSectionsClickable(true);
	connect(this, &QHeaderView::sectionClicked, this, &HeaderBase::slotSectionClicked);
}

int ot::HeaderBase::sizeHintForColumn(int _column) const
{
	int hint = QHeaderView::sizeHintForColumn(_column);

	if (getFilterFeatures(_column))
	{
		hint += m_buttonSize.width() + m_buttonPadding.width();
	}

	return hint;
}

int ot::HeaderBase::sizeHintForRow(int _row) const
{
	int hint = QHeaderView::sizeHintForRow(_row);

	if (getFilterFeatures(_row))
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
	opt.text = model() ? model()->headerData(_logicalIndex, orientation()).toString() : QString();
	opt.state |= QStyle::State_Raised;

	// Call style to draw base header
	style()->drawControl(QStyle::CE_Header, &opt, _painter, this);

	if (!getFilterFeatures(_logicalIndex))
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

	if (getFilterFeatures(_logicalIndex))
	{
		size = size.expandedTo(m_buttonSize + m_buttonPadding);
		size.setWidth(size.width() + m_buttonSize.width());
	}

	return size;
}

void ot::HeaderBase::mousePressEvent(QMouseEvent* _event)
{
	m_lastMousePressPos = _event->pos();
	QHeaderView::mousePressEvent(_event);
}

void ot::HeaderBase::mouseMoveEvent(QMouseEvent* _event)
{
	int ix = logicalIndexAt(_event->pos());
	if (ix >= 0 && getFilterFeatures(ix))
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
	if (m_filterState.hoveredFilter == -1)
	{
		return;
	}

	m_filterState.hoveredFilter = -1;
	update();
	QHeaderView::leaveEvent(_event);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected: Filtering

ot::HeaderFilter::Features ot::HeaderBase::getFilterFeatures(int _logicalIndex) const
{
	return HeaderFilter::Feature::NoFeatuers;
}

QString ot::HeaderBase::getFilterTitle(int _logicalIndex) const
{
	return QString();
}

QStringList ot::HeaderBase::getFilterOptions(int _logicalIndex) const
{
	return QStringList();
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
	HeaderFilter::Features features = getFilterFeatures(_logicalIndex);
	if (!features)
	{
		return;
	}

	QRect rect = filterIconRect(_logicalIndex);
	HeaderFilter filter(_logicalIndex, features, parentWidget());

	filter.setTitle(getFilterTitle(_logicalIndex));
	filter.setOptions(getFilterOptions(_logicalIndex));

	auto it = m_filterState.filterData.find(_logicalIndex);
	if (it != m_filterState.filterData.end())
	{
		filter.updateCheckedState(it->second);
	}

	connect(&filter, &HeaderFilter::sortOrderChanged, this, &HeaderBase::sortOrderChangeRequest);

	filter.showAt(mapToGlobal(rect.bottomLeft()));

	disconnect(&filter, &HeaderFilter::sortOrderChanged, this, &HeaderBase::sortOrderChangeRequest);

	if (filter.wasConfirmed())
	{
		QStringList selectedOptions = filter.saveCheckedState();

		if (selectedOptions.isEmpty())
		{

			m_filterState.activeFilter = (-1);
		}
		else
		{
			m_filterState.activeFilter = _logicalIndex;
		}

		const QStringList& opt = m_filterState.filterData.insert_or_assign(_logicalIndex, std::move(selectedOptions)).first->second;

		try
		{
			filterOptionsChanged(_logicalIndex, opt);
		}
		catch (const std::exception& ex)
		{
			OT_LOG_E("Exception while applying filter: " + std::string(ex.what()));
		}

		update();
	}
}

void ot::HeaderBase::filterOptionsChanged(int _logicalIndex, const QStringList& _selectedOptions)
{

}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Slots

void ot::HeaderBase::sortOrderChangeRequest(int _logicalIndex, Qt::SortOrder _sortOrder)
{}

void ot::HeaderBase::slotSectionClicked(int _logicalIndex)
{
	if (!getFilterFeatures(_logicalIndex))
	{
		return;
		
	}

	QRect iconRect = filterIconRect(_logicalIndex);
	if (iconRect.contains(m_lastMousePressPos))
	{
		m_filterState.pressedFilter = _logicalIndex;
		update();
		showFilterMenu(_logicalIndex);
		m_filterState.pressedFilter = -1;
	}
}