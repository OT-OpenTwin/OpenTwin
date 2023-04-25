#pragma once

// OpenTwin header
#include <openTwin/BlockEditorSharedDatatypes.h>

#include <QtWidgets/qgraphicsview.h>

namespace ot {

	class Block;
	class BlockNetwork;

	class BLOCK_EDITOR_API_EXPORT BlockNetworkEditor : public QGraphicsView {
		Q_OBJECT
	public:
		BlockNetworkEditor();
		virtual ~BlockNetworkEditor();

		BlockNetwork* network(void) { return m_network; };

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

	private:
		bool m_isPressed;
		BlockNetwork* m_network;
	};

}