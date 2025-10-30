// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetBase.h"

// Qt header
#include <QtWidgets/qstyle.h>
#include <QtWidgets/qlayout.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT FlowLayout : public QLayout {
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
