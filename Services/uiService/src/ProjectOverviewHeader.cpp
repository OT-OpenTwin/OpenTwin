// @otlicense
// File: ProjectOverviewHeader.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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
    m_hoveredFilter(-1), m_pressedFilter(-1), m_activeFilter(-1)
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

void ot::ProjectOverviewHeader::setFilterData(const ProjectFilterData& _filterData) {
    // Clear last filter to avoid mismatch in data
    m_lastFilter = ProjectOverviewFilterData();

	m_filterOptions.clear();

    // Project groups

	QStringList groupOptions;
    for (const std::string& group : _filterData.getProjectGroups()) {
        QString groupName = QString::fromStdString(group);
        groupOptions.append(std::move(groupName));
    }
    groupOptions.append(ProjectOverviewFilterData::getEmptyProjectGroupFilterName());
	m_filterOptions.emplace(ColumnIndex::Group, std::move(groupOptions));

    // Project names

	QStringList nameOptions;
    for (const std::string& name : _filterData.getProjectNames()) {
        QString nameStr = QString::fromStdString(name);
		nameOptions.append(std::move(nameStr));
	}
	m_filterOptions.emplace(ColumnIndex::Name, std::move(nameOptions));

    // Project types

    QStringList typeOptions;
    for (const std::string& type : _filterData.getProjectTypes()) {
        QString typeName = QString::fromStdString(type);
        typeOptions.append(std::move(typeName));
	}
    m_filterOptions.emplace(ColumnIndex::Type, std::move(typeOptions));

    // Project tags
	QStringList tagOptions;
    for (const std::string& tag : _filterData.getTags()) {
        QString tagName = QString::fromStdString(tag);
        tagOptions.append(std::move(tagName));
    }
    tagOptions.append(ProjectOverviewFilterData::getEmptyTagsFilterName());
    m_filterOptions.emplace(ColumnIndex::Tags, std::move(tagOptions));

	// Project owners
	QStringList ownerOptions;
    for (const std::string& owner : _filterData.getOwners()) {
        QString ownerName = QString::fromStdString(owner);
        ownerOptions.append(std::move(ownerName));
	}
    m_filterOptions.emplace(ColumnIndex::Owner, std::move(ownerOptions));

	// Project access
	QStringList accessOptions;
    for (const std::string& access : _filterData.getUserGroups()) {
        QString accessName = QString::fromStdString(access);
        accessOptions.append(std::move(accessName));
    }
    accessOptions.append(ProjectOverviewFilterData::getEmptyUserGroupFilterName());
	m_filterOptions.emplace(ColumnIndex::Access, std::move(accessOptions));
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
    else if (m_activeFilter == _logicalIndex) {
        icon = QIcon(cs.getFile(ColorStyleFileEntry::HeaderFilterActiveIcon));
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

void ot::ProjectOverviewHeader::slotSortChanged(int _logicalIndex, Qt::SortOrder _sortOrder) {
    m_overview->sort(_logicalIndex, _sortOrder);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

bool ot::ProjectOverviewHeader::canFilter(int _logicalIndex) const {
	return (_logicalIndex != ColumnIndex::Checked);
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

    ProjectOverviewFilter filter(m_overview, _logicalIndex, _logicalIndex == ColumnIndex::LastAccessed);
	
	// Fill options based on column
    switch (_logicalIndex) {
	case ColumnIndex::Group:
        filter.setTitle("Project Group");
        break;

    case ColumnIndex::Type:
        filter.setTitle("Project Type");
		break;

    case ColumnIndex::Name:
		filter.setTitle("Project Name");
		break;

	case ColumnIndex::Tags:
        filter.setTitle("Project Tags");
        break;

    case ColumnIndex::Owner:
		filter.setTitle("Project Owner");
		break;
    
    case ColumnIndex::Access:
        filter.setTitle("Shared Groups");
		break;

	case ColumnIndex::LastAccessed:
        filter.setTitle("Last Accessed");
		break;

    default:
        OT_LOG_E("Invalid column for filter (" + std::to_string(_logicalIndex) + ")");
        return;
    }

	auto optionsIt = m_filterOptions.find(_logicalIndex);
    if (optionsIt != m_filterOptions.end()) {
        filter.setOptions(optionsIt->second);
    }

	filter.updateCheckedState(m_lastFilter);

	connect(&filter, &ProjectOverviewFilter::sortOrderChanged, this, &ProjectOverviewHeader::slotSortChanged);
	
	filter.showAt(mapToGlobal(rect.bottomLeft()));
    
    disconnect(&filter, &ProjectOverviewFilter::sortOrderChanged, this, &ProjectOverviewHeader::slotSortChanged);
    
    if (filter.isConfirmed()) {
		m_lastFilter = filter.getFilterData();

        if (m_lastFilter.getSelectedFilters().empty()) {
            m_activeFilter = -1;
        }
        else {
            m_activeFilter = m_lastFilter.getLogicalIndex();
        }

        m_overview->filterProjects(m_lastFilter);

        update();
	}
}
