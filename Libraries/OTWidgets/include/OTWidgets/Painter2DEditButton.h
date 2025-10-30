// @otlicense

#pragma once

// OpenTwin header
#include "OTGui/Painter2DDialogFilter.h"
#include "OTWidgets/WidgetBase.h"

// Qt header
#include <QtWidgets/qframe.h>

namespace ot{

	class Painter2D;
	class PushButton;
	class Painter2DPreview;

	class OT_WIDGETS_API_EXPORT Painter2DEditButton : public QFrame, public WidgetBase {
		Q_OBJECT
		OT_DECL_NOCOPY(Painter2DEditButton)
	public:
		//! @brief Creates an instance.
		//! Object takes ownership of the painter.
		Painter2DEditButton(Painter2D* _painter = (Painter2D*)nullptr, QWidget* _parent = (QWidget*)nullptr);

		//! @brief Creates an instance.
		//! Objects creates a copy of the painter.
		Painter2DEditButton(const Painter2D* _painter, QWidget* _parent = (QWidget*)nullptr);
		virtual ~Painter2DEditButton();

		virtual bool eventFilter(QObject* _obj, QEvent* _event) override;

		virtual QWidget* getQWidget() override { return this; };
		virtual const QWidget* getQWidget() const override { return this; };

		PushButton* getPushButton() const { return m_btn; };

		void setPainter(Painter2D* _painter);
		void setPainter(const Painter2D* _painter);
		const Painter2D* getPainter() { return m_painter; };

		void setFilter(const Painter2DDialogFilter& _filter) { m_filter = _filter; };
		const Painter2DDialogFilter& getFilter() const { return m_filter; };

	Q_SIGNALS:
		void painter2DChanged();

	private Q_SLOTS:
		void slotClicked();

	private:
		Painter2DDialogFilter m_filter;
		Painter2D* m_painter;
		PushButton* m_btn;
		QString m_btnTip;
		QString m_btnText;
		Painter2DPreview* m_preview;

		void ini(void);
		void updateText(void);
	};
}

