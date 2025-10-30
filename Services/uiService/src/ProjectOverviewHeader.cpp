// @otlicense

// OpenTwin header
#include "ProjectOverviewHeader.h"
#include "ProjectOverviewWidget.h"
#include "ProjectOverviewFilter.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/GlobalColorStyle.h"

// Qt header
#include <QtGui/qevent.h>
#include <QtGui/qpainter.h>

ot::ProjectOverviewHeader::ProjectOverviewHeader(ProjectOverviewWidget* _overview, QWidget* _parent)
	: QHeaderView(Qt::Orientation::Horizontal, _parent), m_overview(_overview),
    c_buttonSize(14, 14), c_buttonPadding(4, 4),
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
	int hint = QHeaderView::sizeHintForColumn(_column);
    
    if (canFilter(_column)) {
        hint += c_buttonSize.width() + c_buttonPadding.width();
	}

    return hint;
}

void ot::ProjectOverviewHeader::paintSection(QPainter* _painter, const QRect& _rect, int _logicalIndex) const {
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

    if (!canFilter(_logicalIndex)) {
        return;
    }

    const ColorStyle& cs = GlobalColorStyle::instance().getCurrentStyle();

    // Draw filter icon
    QRect iconRect = QRect(
        (_rect.right() - c_buttonSize.width()),
        _rect.center().y() - c_buttonSize.height() / 2,
        c_buttonSize.width(),
        c_buttonSize.height()
    );

    // Choose icon based on hover/press state
    QIcon icon;
    if (m_pressedFilter == _logicalIndex) {
        icon = QIcon(cs.getFile(ColorStyleFileEntry::HeaderFilterPressedIcon));
    }
    else if (m_hoveredFilter == _logicalIndex) {
        icon = QIcon(cs.getFile(ColorStyleFileEntry::HeaderFilterHoverIcon));
    }
    else {
        icon = QIcon(cs.getFile(ColorStyleFileEntry::HeaderFilterIcon));
    }

    if (!icon.isNull()) {
        QPixmap pix = icon.pixmap(c_buttonSize);
        _painter->drawPixmap(iconRect.topLeft(), pix);
    }
}

QSize ot::ProjectOverviewHeader::sectionSizeFromContents(int _logicalIndex) const {
	QSize size = QHeaderView::sectionSizeFromContents(_logicalIndex);
	
    if (canFilter(_logicalIndex)) {
        size = size.expandedTo(c_buttonSize + c_buttonPadding);
        size.setWidth(size.width() + c_buttonSize.width());
    }

	return size;
}

void ot::ProjectOverviewHeader::mousePressEvent(QMouseEvent* _event) {
    int ix = logicalIndexAt(_event->pos());
    if (ix >= 0 && canFilter(ix)) {
        QRect iconRect = filterIconRect(ix);
        if (iconRect.contains(_event->pos())) {
            update();
            QHeaderView::mousePressEvent(_event);

            showFilterMenu(ix);
            return;
        }
        else {
            ix = -1;
        }
    }
    else {
        ix = -1;
    }

    if (ix == m_pressedFilter) {
        return;
    }

	m_pressedFilter = ix;
    update();
    QHeaderView::mousePressEvent(_event);
}

void ot::ProjectOverviewHeader::mouseReleaseEvent(QMouseEvent* _event) {
    if (m_pressedFilter == -1) {
        return;
	}

    m_pressedFilter = -1;
    update();
    QHeaderView::mouseReleaseEvent(_event);
}

void ot::ProjectOverviewHeader::mouseMoveEvent(QMouseEvent* _event) {
    int ix = logicalIndexAt(_event->pos());
    if (ix >= 0 && canFilter(ix)) {
        QRect iconRect = filterIconRect(ix);
        if (!iconRect.contains(_event->pos())) {
            ix = -1;
        }
    }
    else {
        ix = -1;
    }

    if (ix != m_hoveredFilter) {
        m_hoveredFilter = ix;
        update();
        QHeaderView::mouseMoveEvent(_event);
	}
}

void ot::ProjectOverviewHeader::leaveEvent(QEvent* _event) {
    if (m_hoveredFilter == -1 && m_pressedFilter == -1) {
        return;
	}

    m_hoveredFilter = -1;
    m_pressedFilter = -1;
    update();
    QHeaderView::leaveEvent(_event);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Slots

void ot::ProjectOverviewHeader::slotFilterChanged(const ProjectOverviewFilterData& _filterData) {
	m_overview->filterProjects(_filterData);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

bool ot::ProjectOverviewHeader::canFilter(int _logicalIndex) const {
	return (_logicalIndex != ColumnIndex::Checked && _logicalIndex != ColumnIndex::Modified);
}

QRect ot::ProjectOverviewHeader::filterIconRect(int _logicalIndex) const {
    const int pos = sectionViewportPosition(_logicalIndex);
    const int size = sectionSize(_logicalIndex);
    QRect sectionRect(pos, 0, size, height());

    return QRect(
        sectionRect.right() - c_buttonSize.width(),
        sectionRect.center().y() - c_buttonSize.height() / 2,
        c_buttonSize.width(),
        c_buttonSize.height()
    );
}

void ot::ProjectOverviewHeader::showFilterMenu(int _logicalIndex) {
    if (!canFilter(_logicalIndex)) {
        return;
    }

    QRect rect = filterIconRect(_logicalIndex);

    ProjectOverviewFilter filter(m_overview, _logicalIndex);
	QStringList options;
	const std::list<ProjectInformation> allProjects = m_overview->getAllProjects();
    
    bool hasEmptyGroups = false;
    bool hasEmptyTags = false;

	// Fill options based on column
    switch (_logicalIndex) {
    case ColumnIndex::Type:
        filter.setTitle("Project Type");
        for (const ProjectInformation& proj : allProjects) {
            const QString type = QString::fromStdString(proj.getProjectType());
            if (!options.contains(type)) {
                options.append(type);
            }
		}
		break;

    case ColumnIndex::Name:
		filter.setTitle("Project Name");
        for (const ProjectInformation& proj : allProjects) {
            const QString name = QString::fromStdString(proj.getProjectName());
            if (!options.contains(name)) {
                options.append(name);
            }
        }
		break;

	case ColumnIndex::Tags:
        filter.setTitle("Project Tags");
        for (const ProjectInformation& proj : allProjects) {
			if (proj.getTags().empty()) {
				hasEmptyTags = true;
			}
            for (const std::string& tag : proj.getTags()) {
                const QString tagName = QString::fromStdString(tag);
                if (!options.contains(tagName)) {
                    options.append(tagName);
                }
			}
        }
        break;

    case ColumnIndex::Owner:
		filter.setTitle("Project Owner");
        for (const ProjectInformation& proj : allProjects) {
            const QString owner = QString::fromStdString(proj.getUserName());
            if (!options.contains(owner)) {
                options.append(owner);
            }
		}
		break;
    
    case ColumnIndex::Access:
        filter.setTitle("Shared Groups");
        for (const ProjectInformation& proj : allProjects) {
            if (proj.getUserGroups().empty()) {
				hasEmptyGroups = true;
            }
            for (const std::string& group : proj.getUserGroups()) {
                const QString groupName = QString::fromStdString(group);
                if (!options.contains(groupName)) {
                    options.append(groupName);
                }
            }
        }
		break;

    default:
        OT_LOG_E("Invalid column for filter (" + std::to_string(_logicalIndex) + ")");
        return;
    }

	options.sort();

    if (hasEmptyGroups) {
        options.append(ProjectOverviewFilterData::getEmptyGroupFilterName());
    }
    if (hasEmptyTags) {
        options.append(ProjectOverviewFilterData::getEmptyTagsFilterName());
	}

	filter.setOptions(options);
	filter.updateCheckedStatesFromData(m_lastFilter);

    m_overview->filterProjects(filter.getFilterData());

	connect(&filter, &ProjectOverviewFilter::filterChanged, this, &ProjectOverviewHeader::slotFilterChanged);

	filter.exec(mapToGlobal(rect.bottomLeft()));
    
    disconnect(&filter, &ProjectOverviewFilter::filterChanged, this, &ProjectOverviewHeader::slotFilterChanged);

    if (filter.isConfirmed()) {
		m_lastFilter = filter.getFilterData();
	}
    else {
        m_lastFilter = ProjectOverviewFilterData();
    }

    m_overview->filterProjects(m_lastFilter);
}
