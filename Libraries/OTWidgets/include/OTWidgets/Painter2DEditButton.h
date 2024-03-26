//! @file Painter2DEditButton.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/QWidgetInterface.h"

// Qt header
#include <QtCore/qobject.h>

namespace ot{

	class Painter2D;
	class PushButton;
	class Painter2DPreview;

	class OT_WIDGETS_API_EXPORT Painter2DEditButton : public QObject, public QWidgetInterface {
		Q_OBJECT
		OT_DECL_NOCOPY(Painter2DEditButton)
	public:
		//! @brief Creates an instance.
		//! Object takes ownership of the painter.
		Painter2DEditButton(Painter2D* _painter = (Painter2D*)nullptr);
		//! @brief Creates an instance.
		//! Objects creates a copy of the painter.
		Painter2DEditButton(const Painter2D* _painter);
		virtual ~Painter2DEditButton();

		virtual QWidget* getQWidget(void) override { return m_rootWidget; };
		PushButton* getButton(void) const { return m_btn; };

		void setPainter(Painter2D* _painter);
		void setPainter(const Painter2D* _painter);
		const Painter2D* getPainter(void) { return m_painter; };

	Q_SIGNALS:
		void painter2DChanged(void);

	private Q_SLOTS:
		void slotClicked(void);

	private:
		QWidget* m_rootWidget;
		Painter2D* m_painter;
		PushButton* m_btn;
		Painter2DPreview* m_preview;

		void updateText(void);
	};
}

