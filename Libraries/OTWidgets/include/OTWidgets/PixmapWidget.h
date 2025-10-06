//! @file PixmapWidget.h
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/WidgetBase.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtGui/qpixmap.h>
#include <QtWidgets/qframe.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT PixmapWidget : public QFrame, public WidgetBase {
		Q_OBJECT
		OT_DECL_NOCOPY(PixmapWidget)
		OT_DECL_NOMOVE(PixmapWidget)
	public:
		PixmapWidget();
		PixmapWidget(const QPixmap& _pixmap);
		virtual ~PixmapWidget();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Virtual methods

		virtual QWidget* getQWidget(void) override { return this; };
		virtual const QWidget* getQWidget(void) const override { return this; };

		virtual void paintEvent(QPaintEvent* _event) override;

		virtual QSize sizeHint(void) const override;

		virtual void wheelEvent(QWheelEvent* _event) override;

		virtual void keyPressEvent(QKeyEvent* _event) override;

		virtual void mousePressEvent(QMouseEvent* _event) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Pixmap manipulation

		void setPixmap(const QPixmap& _pixmap);
		const QPixmap& getPixmap(void) const { return m_pixmap; };

		void setPixmapResizable(bool _resizable) { m_enabledResizing = _resizable; };
		bool getPixmapResizable(void) const { return m_enabledResizing; };

	Q_SIGNALS:
		void imagePixedClicked(const QPoint& _px);

	protected:
		virtual void resizeEvent(QResizeEvent* _event) override;

		virtual QSize calculateScaledPixmap(const QSize& _newPreferredSize);

	private:
		bool m_enabledResizing;
		bool m_userResized;
		QPixmap m_pixmap;
		QPixmap m_scaledPixmap;
		QSizeF m_scaleFactors;

		void updateScaleFactors(void);
	};

}