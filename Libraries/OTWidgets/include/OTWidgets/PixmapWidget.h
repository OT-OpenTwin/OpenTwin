//! @file PixmapWidget.h
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/QWidgetInterface.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtGui/qpixmap.h>
#include <QtWidgets/qframe.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT PixmapWidget : public QFrame, public QWidgetInterface {
		Q_OBJECT
		OT_DECL_NOCOPY(PixmapWidget)
		OT_DECL_NOMOVE(PixmapWidget)
	public:
		PixmapWidget();
		PixmapWidget(const QPixmap& _pixmap);
		virtual ~PixmapWidget();

		virtual QWidget* getQWidget(void) override { return this; };
		virtual const QWidget* getQWidget(void) const override { return this; };

		virtual void paintEvent(QPaintEvent* _event) override;

		virtual QSize sizeHint(void) const override;

		virtual void wheelEvent(QWheelEvent* _event) override;

		virtual void keyPressEvent(QKeyEvent* _event) override;

		virtual void mousePressEvent(QMouseEvent* _event) override;

		void setPixmap(const QPixmap& _pixmap);
		const QPixmap& getPixmap(void) const { return m_pixmap; };
		
		void scalePixmap(const QSize& _newSize);

		void setEnableResizing(bool _enable) { m_enabledResizing = _enable; };
		bool getEnableResizing(void) const { return m_enabledResizing; };

	Q_SIGNALS:
		void imagePixedClicked(const QPoint& _px);

	protected:
		virtual void resizeEvent(QResizeEvent* _event) override;

	private:
		bool m_enabledResizing;
		QPixmap m_pixmap;
		QSize m_fixedSize;
		QPixmap m_scaledPixmap;
		QSize m_scaleFactors;
	};

}