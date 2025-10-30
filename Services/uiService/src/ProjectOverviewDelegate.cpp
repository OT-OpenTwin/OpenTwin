// @otlicense
// File: ProjectOverviewDelegate.cpp
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
