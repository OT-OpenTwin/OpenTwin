// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/Widgets/Label.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT InteractiveLabel : public Label
	{
		Q_OBJECT
		OT_DECL_NOCOPY(InteractiveLabel)
		OT_DECL_NOMOVE(InteractiveLabel)
		OT_DECL_NODEFAULT(InteractiveLabel)
	public:
		explicit InteractiveLabel(QWidget* _parentWidget);
		explicit InteractiveLabel(const QString& _text, QWidget* _parentWidget);
		virtual ~InteractiveLabel() {};

	Q_SIGNALS:
		void mouseClicked();
		void mouseDoubleClicked();
		void mouseEntered();
		void mouseLeft();

	protected:
		virtual void mousePressEvent(QMouseEvent* _event) override;
		virtual void mouseReleaseEvent(QMouseEvent* _event) override;
		virtual void mouseDoubleClickEvent(QMouseEvent* _event) override;
		virtual void enterEvent(QEnterEvent* _event) override;
		virtual void leaveEvent(QEvent* _event) override;

	private:
		bool m_mouseIsPressed;
	};
}