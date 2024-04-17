//! @file ImagePreview.h
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/QWidgetInterface.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtGui/qimage.h>
#include <QtWidgets/qframe.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT ImagePreview : public QFrame, public QWidgetInterface {
	public:
		ImagePreview();
		ImagePreview(const QImage& _image);
		virtual ~ImagePreview();

		virtual QWidget* getQWidget(void) override { return this; };

		virtual void paintEvent(QPaintEvent* _event) override;

		virtual QSize sizeHint(void) const override;

		virtual void wheelEvent(QWheelEvent* _event) override;

		virtual void keyPressEvent(QKeyEvent* _event) override;

		void setImage(const QImage& _image);
		const QImage& image(void) const { return m_image; };

	private:
		QSize m_size;
		QImage m_image;
	};

}