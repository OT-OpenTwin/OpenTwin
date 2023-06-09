//! @file GraphicsView.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtWidgets/qgraphicsview.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsView : public QGraphicsView {
		Q_OBJECT
	public:
		GraphicsView();
		virtual ~GraphicsView();

		void resetView(void);
		void viewAll(void);

	protected slots:
		virtual void wheelEvent(QWheelEvent* _event) override;
		virtual void enterEvent(QEvent* _event) override;
		virtual void mousePressEvent(QMouseEvent* _event) override;
		virtual void mouseReleaseEvent(QMouseEvent* _event) override;
		virtual void mouseMoveEvent(QMouseEvent* _event) override;
		virtual void keyPressEvent(QKeyEvent* _event) override;
		virtual void keyReleaseEvent(QKeyEvent* _event) override;
		virtual void resizeEvent(QResizeEvent* _event) override;

	protected:
		virtual void mousePressedMoveEvent(QMouseEvent* _event) {};

	private:
		bool m_isPressed;

	};

}