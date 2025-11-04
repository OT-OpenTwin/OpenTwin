// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetBase.h"

// Qt header
#include <QtWidgets/qgraphicsview.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT PixmapEditWidget : public QGraphicsView, public WidgetBase {
		Q_OBJECT
		OT_DECL_NOCOPY(PixmapEditWidget)
		OT_DECL_NOMOVE(PixmapEditWidget)
	public:
        explicit PixmapEditWidget(QWidget* parent = (QWidget*)nullptr);

		virtual QWidget* getQWidget() override { return this; };
		virtual const QWidget* getQWidget() const override { return this; };

        // ###########################################################################################################################################################################################################################################################################################################################

        // Setter / Getter

        void setPixmap(const QPixmap& _pixmap);
		void setResultSize(const QSize& _size) { m_resultSize = _size; };
        QPixmap getResultPixmap() const;
        void resetView();

        // ###########################################################################################################################################################################################################################################################################################################################

		// Protected: Event handlers

    protected:
        void wheelEvent(QWheelEvent* _event) override;
        void mousePressEvent(QMouseEvent* _event) override;
        void mouseMoveEvent(QMouseEvent* _event) override;
        void mouseReleaseEvent(QMouseEvent* _event) override;
        void resizeEvent(QResizeEvent* _event) override;

        // ###########################################################################################################################################################################################################################################################################################################################

        // Private: Helper

    private:
        void updateCropRect(const QPointF& _start, const QPointF& _end);
        void updateCropRectLineWidth();
		bool isOnCropRect(const QPointF& _scenePos) const;

        QGraphicsScene* m_scene;
        QGraphicsPixmapItem* m_pixmapItem;
        QGraphicsRectItem* m_cropRect;

        QSize m_resultSize;
        QPointF m_lastMousePos;

		double m_zoomFactor;

        enum class State : uint64_t {
            None          = 0 << 0,
            Panning       = 1 << 0,
			SelectingCrop = 1 << 1,
            OnCropRect    = 1 << 2
		};
        typedef Flags<State> StateFlags;

		StateFlags m_state;
        
        OT_ADD_FRIEND_FLAG_FUNCTIONS(State, StateFlags)
    };

}