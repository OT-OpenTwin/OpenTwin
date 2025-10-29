// @otlicense

//! @file CartesianPlotLegend.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

/*
// OpenTwin header
#include "OTWidgets/Label.h"
#include "OTWidgets/CartesianPlot.h"
#include "OTWidgets/CartesianPlotLegend.h"
#include "OTWidgets/CartesianPlotLegendView.h"

// Qwt header
#include <qwt_text.h>
#include <qwt_graphic.h>
#include <qwt_painter.h>
#include <qwt_legend_data.h>

// Qt header
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qscrollbar.h>

// std header

ot::CartesianPlotLegend::CartesianPlotLegend(CartesianPlot* _owner, QWidget* _parent) :
    QwtAbstractLegend(_parent), m_owner(_owner), m_view(nullptr)
{
    m_view = new CartesianPlotLegendView(this);
}

ot::CartesianPlotLegend::~CartesianPlotLegend() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Virtual methods

void ot::CartesianPlotLegend::renderLegend(QPainter* _painter, const QRectF& _rect, bool _fillBackground) const {
    if (m_legendData.empty()) {
        return;
    }

    if (_fillBackground) {
        if (autoFillBackground() || testAttribute(Qt::WA_StyledBackground)) {
            QwtPainter::drawBackgound(_painter, _rect, this);
        }
    }

    const QVBoxLayout* legendLayout = m_view->contentsLayout();
    if (legendLayout == nullptr) {
        return;
    }

    const QMargins m = contentsMargins();

    QRect layoutRect;
    
    layoutRect.setLeft(std::ceil(_rect.left()) + m.left());
    layoutRect.setTop(std::ceil(_rect.top()) + m.top());
    layoutRect.setRight(std::floor(_rect.right()) - m.right());
    layoutRect.setBottom(std::floor(_rect.bottom()) - m.bottom());

    

    uint numCols = legendLayout->columnsForWidth(layoutRect.width());
    const QList<QRect> itemRects = legendLayout->layoutItems(layoutRect, numCols);

    int index = 0;

    for (int i = 0; i < legendLayout->count(); i++) {
        QLayoutItem* item = legendLayout->itemAt(i);
        QWidget* w = item->widget();
        if (w) {
            painter->save();

            painter->setClipRect(itemRects[index], Qt::IntersectClip);
            renderItem(painter, w, itemRects[index], fillBackground);

            index++;
            painter->restore();
        }
    }
}

bool ot::CartesianPlotLegend::isEmpty() const {
	return m_legendData.empty();
}

int ot::CartesianPlotLegend::scrollExtent(Qt::Orientation _orientation) const {
    int extent = 0;

    if (_orientation == Qt::Horizontal) {
        extent = m_view->verticalScrollBar()->sizeHint().width();
    }
    else {
        extent = m_view->horizontalScrollBar()->sizeHint().height();
    }

    return extent;
}

void ot::CartesianPlotLegend::updateLegend(const QVariant& _itemInfo, const QList<QwtLegendData>& _data) {
    std::list<Label*> widgetList = this->getItemWidgets(_itemInfo);

    if (widgetList.size() != _data.size()) {
        QLayout* contentsLayout = m_view->contentsLayout();

        while (widgetList.size() > _data.size()) {
            Label* w = widgetList.back();
			widgetList.pop_back();

            contentsLayout->removeWidget(w);

            // updates might be triggered by signals from the legend widget
            // itself. So we better don't delete it here.

            w->hide();
            w->deleteLater();
        }

        for (int i = widgetList.size(); i < _data.size(); i++) {
            Label* widget = this->createItemWidget(_data[i]);

            contentsLayout->addWidget(widget);

            if (isVisible()) {
                // QLayout does a delayed show, with the effect, that
                // the size hint will be wrong, when applications
                // call replot() right after changing the list
                // of plot items. So we better do the show now.

                widget->setVisible(true);
            }

            widgetList.push_back(widget);
        }

        if (widgetList.empty()) {
			this->removeLegendData(_itemInfo);
        }
        else {
			this->addLegendData(_itemInfo, widgetList);
        }
    }

	OTAssert(widgetList.size() == _data.size(), "Widget list size does not match data size");

	int ct = 0;
    for (Label* w : widgetList) {
		this->updateItemWidget(w, _data[ct++]);
    }
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Slots

void ot::CartesianPlotLegend::slotItemClicked() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

void ot::CartesianPlotLegend::addLegendData(const QVariant& _itemInfo, const std::list<Label*>& _widgets) {
    for (ItemEntry& entry : m_legendData) {
		if (entry.itemInfo == _itemInfo) {
			entry.widgets = _widgets;
			return;
		}
    }

	ItemEntry newEntry;
	newEntry.itemInfo = _itemInfo;
	newEntry.widgets = _widgets;
    m_legendData.push_back(std::move(newEntry));
}

void ot::CartesianPlotLegend::removeLegendData(const QVariant& _itemInfo) {
    for (auto it = m_legendData.begin(); it != m_legendData.end(); it++) {
		if (it->itemInfo == _itemInfo) {
			m_legendData.erase(it);
			break;
		}
    }
}

std::list<ot::Label*> ot::CartesianPlotLegend::getItemWidgets(const QVariant& _itemInfo) const {
    if (_itemInfo.isValid()) {
        for (const ItemEntry& entry : m_legendData) {
            if (entry.itemInfo == _itemInfo) {
                return entry.widgets;
            }
        }
    }
	return std::list<Label*>();
}

ot::Label* ot::CartesianPlotLegend::createItemWidget(const QwtLegendData& _data) const {
    OT_UNUSED(_data);
    
    Label* label = new Label;
    label->setFocusPolicy(Qt::NoFocus);

    connect(label, &Label::mouseClicked, this, &CartesianPlotLegend::slotItemClicked);

    return label;
}

void ot::CartesianPlotLegend::updateItemWidget(Label* _widget, const QwtLegendData& _data) const {
    if (_widget) {
        const bool doUpdate = updatesEnabled();
        if (doUpdate) {
            _widget->setUpdatesEnabled(false);
        }

        _widget->setText(_data.title().text());
        _widget->setPixmap(_data.icon().toPixmap());

        if (doUpdate) {
            _widget->setUpdatesEnabled(true);
        }
    }
}
*/