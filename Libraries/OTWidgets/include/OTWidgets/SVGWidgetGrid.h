//! @file SVGWidgetGrid.h
//! @author Alexander Kuester (alexk95)
//! @date February 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/QWidgetInterface.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtWidgets/qscrollarea.h>

class QGridLayout;

namespace ot {

	class SVGWidget;

	class OT_WIDGETS_API_EXPORT SVGWidgetGrid : public QScrollArea, public QWidgetInterface {
	public:
		SVGWidgetGrid();

		void fillFromPath(const QString& _rootPath);

		void clear(void);

		virtual QWidget* getQWidget(void) override { return this; };

		virtual const QWidget* getQWidget(void) const override { return this; };

	protected:
		virtual void resizeEvent(QResizeEvent* _event) override;

	private:
		void rebuildGrid(const QSize& _newSize);

		QGridLayout* m_layout;

		QSize m_itemSize;
	};

	std::list<SVGWidget*> m_widgets;

}