//! @file CartesianPlotLegendView.h
//! @author Alexander Kuester (alexk95)
//! @date June 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtWidgets/qscrollarea.h>

class QWidget;
class QVBoxLayout;

namespace ot {

	class CartesianPlotLegendView : public QScrollArea {
		OT_DECL_NODEFAULT(CartesianPlotLegendView)
		OT_DECL_NOCOPY(CartesianPlotLegendView)
		OT_DECL_NOMOVE(CartesianPlotLegendView)
	public:
		explicit CartesianPlotLegendView(QWidget* _parent);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		QWidget* contentsWidget() const { return m_contentsWidget; };
		QVBoxLayout* contentsLayout() const { return m_contentsLayout; };

	private:
		QWidget* m_contentsWidget;
		QVBoxLayout* m_contentsLayout;

	};

}