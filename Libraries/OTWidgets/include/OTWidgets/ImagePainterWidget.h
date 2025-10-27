// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetBase.h"

// Qt header
#include <QtWidgets/qframe.h>

namespace ot {

	class ImagePainter;

	class OT_WIDGETS_API_EXPORT ImagePainterWidget : public QFrame, public WidgetBase {
		OT_DECL_NOCOPY(ImagePainterWidget)
		OT_DECL_NOMOVE(ImagePainterWidget)
	public:
		ImagePainterWidget(QWidget* _parent = (QWidget*)nullptr);
		virtual ~ImagePainterWidget();

		virtual QWidget* getQWidget() override { return this; };
		virtual const QWidget* getQWidget() const override { return this; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setPainter(ImagePainter* _painter);
		const ImagePainter* getPainter() const { return m_painter; };

		void setInteractive(bool _interactive) { m_interactive = _interactive; };
		bool isInteractive() const { return m_interactive; };

		virtual QSize sizeHint() const override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Proetected: Events

	protected:
		virtual void paintEvent(QPaintEvent* _event) override;
		virtual void enterEvent(QEnterEvent* _event) override;
		virtual void leaveEvent(QEvent* _event) override;

	private:
		ImagePainter* m_painter;
		bool m_interactive;
	};

}