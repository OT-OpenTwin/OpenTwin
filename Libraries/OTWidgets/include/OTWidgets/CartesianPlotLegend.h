// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// Qwt header
#include <qwt_legend.h>
#include <qwt_legend_data.h>
#include <qwt_abstract_legend.h>

// Qt header
#include <QtCore/qlist.h>
#include <QtCore/qvariant.h>

// std header
#include <list>

class QRectF;
class QWidget;
class QPainter;
class QScrollArea;

namespace ot {

	class Label;
	class CartesianPlot;
	class CartesianPlotLegendView;

	class OT_WIDGETS_API_EXPORT CartesianPlotLegend : public QwtLegend {
	public:
		CartesianPlotLegend(CartesianPlot* _owner) {};
	};

	/*
	class OT_WIDGETS_API_EXPORT CartesianPlotLegend : public QwtAbstractLegend {
		Q_OBJECT
		OT_DECL_NOCOPY(CartesianPlotLegend)
		OT_DECL_NOMOVE(CartesianPlotLegend)
		OT_DECL_NODEFAULT(CartesianPlotLegend)
    public:
        CartesianPlotLegend(CartesianPlot* _owner, QWidget* _parent = (QWidget*)nullptr);
        virtual ~CartesianPlotLegend();

        // ###########################################################################################################################################################################################################################################################################################################################

		// Virtual methods

		//! @brief Render the legend into a given rectangle.
		//! @param _painter Painter.
		//! @param _rect Bounding rectangle.
		//! @param _fillBackground When true, fill rect with the widget background.
		//! @ref renderLegend() is used by QwtPlotRenderer
        void renderLegend(QPainter* _painter, const QRectF& _rect, bool _fillBackground) const override;

        //! @return True, when no plot item is inserted
        bool isEmpty() const override;

        int scrollExtent(Qt::Orientation _orientation) const override;

        //! @brief Update the entries for a plot item
        //! @param itemInfo Info about an item
        //! @param data List of legend entry attributes for the  item
        void updateLegend(const QVariant& _itemInfo, const QList<QwtLegendData>& _data) override;

	private Q_SLOTS:
		void slotItemClicked();
		
	private:
		struct ItemEntry {
			QVariant itemInfo;
			std::list<Label*> widgets;
		};

		CartesianPlot* m_owner;
        CartesianPlotLegendView* m_view;

		std::list<ItemEntry> m_legendData;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Helper

		void addLegendData(const QVariant& _itemInfo, const std::list<Label*>& _widgets);
		void removeLegendData(const QVariant& _itemInfo);
		std::list<Label*> getItemWidgets(const QVariant& _itemInfo) const;

		Label* createItemWidget(const QwtLegendData& _data) const;
		void updateItemWidget(Label* _widget, const QwtLegendData& _data) const;

	};
	*/
}