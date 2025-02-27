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
#include <QtCore/qtimer.h>
#include <QtWidgets/qscrollarea.h>

// std header
#include <vector>

class QGridLayout;

namespace ot {

	class SVGWidget;

	class OT_WIDGETS_API_EXPORT SVGWidgetGrid : public QScrollArea, public QWidgetInterface {
		Q_OBJECT
	public:
		SVGWidgetGrid();

		void fillFromPath(const QString& _rootPath);

		void clear(void);

		virtual QWidget* getQWidget(void) override { return this; };

		virtual const QWidget* getQWidget(void) const override { return this; };

		void setItemSize(int _widthAndHeight) { this->setItemSize(QSize(_widthAndHeight, _widthAndHeight)); };
		void setItemSize(int _width, int _height) { this->setItemSize(QSize(_width, _height)); };
		void setItemSize(const QSize& _size);

	protected:
		virtual void resizeEvent(QResizeEvent* _event) override;

	private Q_SLOTS:
		void slotRebuildGridStep(void);

	private:
		void rebuildGrid(const QSize& _newSize);
		void resetRebuildInfo(void);
		
		QGridLayout* m_layout;

		QSize m_itemSize;

		QTimer m_rebuildTimer;
		
		struct RebuildInfo {
			size_t widgetIx;
			int row;
			int col;
			int cols;
			std::vector<QString> filePaths;
		};

		RebuildInfo m_rebuildInfo;

		std::vector<SVGWidget*> m_widgets;
	};

}