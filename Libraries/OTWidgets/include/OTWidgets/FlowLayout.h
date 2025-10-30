// @otlicense
// File: FlowLayout.h
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

// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetBase.h"

// Qt header
#include <QtWidgets/qstyle.h>
#include <QtWidgets/qlayout.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT FlowLayout : public QLayout {
		OT_DECL_NOMOVE(FlowLayout)
		OT_DECL_NOCOPY(FlowLayout)
	public:
        explicit FlowLayout(QWidget* _parent = (QWidget*)nullptr);
        ~FlowLayout();

        void addItem(QLayoutItem* _item) override;
        int horizontalSpacing() const;
        int verticalSpacing() const;
        Qt::Orientations expandingDirections() const override;
        bool hasHeightForWidth() const override;
        int heightForWidth(int _width) const override;
        int count() const override;
        QLayoutItem* itemAt(int _index) const override;
        QSize minimumSize() const override;
        void setGeometry(const QRect& _rect) override;
        QSize sizeHint() const override;
        QLayoutItem* takeAt(int _index) override;

    private:
        int doLayout(const QRect& _rect, bool _testOnly) const;
        int smartSpacing(QStyle::PixelMetric _pm) const;

        QList<QLayoutItem*> itemList;
        int m_hSpace;
        int m_vSpace;
	};

}
