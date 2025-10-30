// @otlicense

// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: BSD-3-Clause

#include "OTWidgets/FlowLayout.h"

ot::FlowLayout::FlowLayout(QWidget* _parent)
    : QLayout(_parent), m_hSpace(-1), m_vSpace(-1)
{
    
}

ot::FlowLayout::~FlowLayout() {
    QLayoutItem* item;
    while ((item = takeAt(0))) {
        delete item;
    }
}

void ot::FlowLayout::addItem(QLayoutItem* _item) {
    itemList.append(_item);
}

int ot::FlowLayout::horizontalSpacing() const {
    if (m_hSpace >= 0) {
        return m_hSpace;
    }
    else {
        return smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
    }
}

int ot::FlowLayout::verticalSpacing() const {
    if (m_vSpace >= 0) {
        return m_vSpace;
    }
    else {
        return smartSpacing(QStyle::PM_LayoutVerticalSpacing);
    }
}

int ot::FlowLayout::count() const {
    return itemList.size();
}

QLayoutItem* ot::FlowLayout::itemAt(int _index) const {
    return itemList.value(_index);
}

QLayoutItem* ot::FlowLayout::takeAt(int _index) {
    if (_index >= 0 && _index < itemList.size()) {
        return itemList.takeAt(_index);
    }
    else {
        return nullptr;
    }
}

Qt::Orientations ot::FlowLayout::expandingDirections() const {
    return { };
}

bool ot::FlowLayout::hasHeightForWidth() const {
    return true;
}

int ot::FlowLayout::heightForWidth(int _width) const {
    return doLayout(QRect(0, 0, _width, 0), true);
}

void ot::FlowLayout::setGeometry(const QRect& rect) {
    QLayout::setGeometry(rect);
    doLayout(rect, false);
}

QSize ot::FlowLayout::sizeHint() const {
    return minimumSize();
}

QSize ot::FlowLayout::minimumSize() const {
    QSize size;
    for (const QLayoutItem* item : std::as_const(itemList)) {
        size = size.expandedTo(item->minimumSize());
    }

    const QMargins margins = contentsMargins();
    size += QSize(margins.left() + margins.right(), margins.top() + margins.bottom());
    return size;
}

int ot::FlowLayout::doLayout(const QRect& rect, bool testOnly) const {
    int left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    QRect effectiveRect = rect.adjusted(+left, +top, -right, -bottom);
    int x = effectiveRect.x();
    int y = effectiveRect.y();
    int lineHeight = 0;
    
    for (QLayoutItem* item : std::as_const(itemList)) {
        const QWidget* wid = item->widget();
        
        int spaceX = horizontalSpacing();
        if (spaceX == -1) {
            spaceX = wid->style()->layoutSpacing(QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Horizontal);
        }
        int spaceY = verticalSpacing();
        if (spaceY == -1) {
            spaceY = wid->style()->layoutSpacing(QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Vertical);
        }

        int nextX = x + item->sizeHint().width() + spaceX;
        if (nextX - spaceX > effectiveRect.right() && lineHeight > 0) {
            x = effectiveRect.x();
            y = y + lineHeight + spaceY;
            nextX = x + item->sizeHint().width() + spaceX;
            lineHeight = 0;
        }

        if (!testOnly) {
            item->setGeometry(QRect(QPoint(x, y), item->sizeHint()));
        }

        x = nextX;
        lineHeight = qMax(lineHeight, item->sizeHint().height());
    }

    return y + lineHeight - rect.y() + bottom;
}

int ot::FlowLayout::smartSpacing(QStyle::PixelMetric pm) const {
    QObject* parent = this->parent();
    if (!parent) {
        return -1;
    }
    else if (parent->isWidgetType()) {
        QWidget* pw = static_cast<QWidget*>(parent);
        return pw->style()->pixelMetric(pm, nullptr, pw);
    }
    else {
        return static_cast<QLayout*>(parent)->spacing();
    }
}