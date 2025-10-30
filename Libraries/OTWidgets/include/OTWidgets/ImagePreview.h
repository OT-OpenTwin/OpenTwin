// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetBase.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtGui/qimage.h>
#include <QtWidgets/qframe.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT ImagePreview : public QFrame, public WidgetBase {
		Q_OBJECT
	public:
		ImagePreview();
		ImagePreview(const QImage& _image);
		virtual ~ImagePreview();

		virtual QWidget* getQWidget(void) override { return this; };
		virtual const QWidget* getQWidget(void) const override { return this; };

		virtual void paintEvent(QPaintEvent* _event) override;

		virtual QSize sizeHint(void) const override;

		virtual void wheelEvent(QWheelEvent* _event) override;

		virtual void keyPressEvent(QKeyEvent* _event) override;

		virtual void mousePressEvent(QMouseEvent* _event) override;

		void setImage(const QImage& _image);
		const QImage& getImage(void) const { return m_image; };

		void setImageMargins(const QMargins& _margins);
		const QMargins& getImageMargins(void) const { return m_imageMargins; };

		void setEnableResizing(bool _enable) { m_enabledResizing = _enable; };
		bool getEnableResizing(void) const { return m_enabledResizing; };

	Q_SIGNALS:
		void imagePixedClicked(const QPoint& _px);

	private:
		bool m_enabledResizing;
		QSize m_size;
		QImage m_image;
		QMargins m_imageMargins;
	};

}