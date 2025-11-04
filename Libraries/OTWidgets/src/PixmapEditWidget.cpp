// @otlicense

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/PixmapEditWidget.h"
#include "OTWidgets/GlobalColorStyle.h"

// Qt header
#include <QtGui/qevent.h>
#include <QtWidgets/qscrollbar.h>
#include <QtWidgets/qgraphicsitem.h>
#include <QtWidgets/qgraphicsscene.h>

ot::PixmapEditWidget::PixmapEditWidget(QWidget* _parent)
    : QGraphicsView(_parent), m_scene(new QGraphicsScene(this)), m_pixmapItem(new QGraphicsPixmapItem()),
	m_cropRect(new QGraphicsRectItem()), m_resultSize(256, 256), m_state(State::None), m_zoomFactor(1.0)
{
    setScene(m_scene);
    m_scene->addItem(m_pixmapItem);

    const ColorStyle& cs = GlobalColorStyle::instance().getCurrentStyle();
    m_cropRect->setPen(QPen(cs.getValue(ColorStyleValueEntry::GraphicsItemRubberband).toBrush(), 2., Qt::DashLine));
    m_cropRect->setVisible(false);
    m_scene->addItem(m_cropRect);

    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setDragMode(QGraphicsView::NoDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void ot::PixmapEditWidget::setPixmap(const QPixmap& _pixmap) {
    m_pixmapItem->setPixmap(_pixmap);
    m_scene->setSceneRect(_pixmap.rect());
    resetView();
    updateCropRectLineWidth();
}

QPixmap ot::PixmapEditWidget::getResultPixmap() const {
    if (!m_pixmapItem->pixmap().isNull() && m_cropRect->isVisible()) {
        QRectF cropSceneRect = m_cropRect->rect();
        QPixmap source = m_pixmapItem->pixmap();
        QRect cropRect = cropSceneRect.toRect().intersected(source.rect());
        QPixmap cropped = source.copy(cropRect);
        return cropped.scaled(m_resultSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    // Fallback to full scaled image
    return m_pixmapItem->pixmap().scaled(m_resultSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

void ot::PixmapEditWidget::resetView() {
    fitInView(m_pixmapItem, Qt::KeepAspectRatio);
    m_zoomFactor = 1.0;
    m_cropRect->setVisible(false);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected: Event handlers

void ot::PixmapEditWidget::wheelEvent(QWheelEvent* _event) {
    const double scaleFactor = 1.15;
    if (_event->angleDelta().y() > 0) {
        scale(scaleFactor, scaleFactor);
        m_zoomFactor *= scaleFactor;
    }
    else {
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
        m_zoomFactor /= scaleFactor;
    }

    updateCropRectLineWidth();
}

void ot::PixmapEditWidget::mousePressEvent(QMouseEvent* _event) {
    QPointF scenePos = mapToScene(_event->pos());

    if (_event->button() == Qt::MiddleButton) {
        m_state.clear();
        m_state.set(State::Panning);
		m_state.set(State::OnCropRect, isOnCropRect(scenePos));

        m_lastMousePos = _event->pos();
        setCursor(Qt::ClosedHandCursor);
    }
    else if (_event->button() == Qt::LeftButton) {
        m_state.clear();
		m_state.set(State::SelectingCrop);
        
        updateCropRect(scenePos, scenePos);
        m_cropRect->setVisible(true);
    }
    QGraphicsView::mousePressEvent(_event);
}

void ot::PixmapEditWidget::mouseMoveEvent(QMouseEvent* _event) {
    if (m_state & State::Panning) {
        QPointF delta = mapToScene(m_lastMousePos.toPoint()) - mapToScene(_event->pos());

        if (m_state & State::OnCropRect) {
            // Move crop
            QRectF rect = m_cropRect->rect();
            m_cropRect->setRect(rect.translated(-delta));
		}
        else {
            // Pan view
            horizontalScrollBar()->setValue(horizontalScrollBar()->value() + delta.x());
            verticalScrollBar()->setValue(verticalScrollBar()->value() + delta.y());
        }

        m_lastMousePos = _event->pos();
    }
    else if (m_state & State::SelectingCrop) {
        QPointF scenePos = mapToScene(_event->pos());
        QRectF rect = m_cropRect->rect();
        updateCropRect(m_cropRect->rect().topLeft(), scenePos);
    }
    QGraphicsView::mouseMoveEvent(_event);
}

void ot::PixmapEditWidget::mouseReleaseEvent(QMouseEvent* _event) {
    if (_event->button() == Qt::MiddleButton && m_state & State::Panning) {
		m_state.remove(State::Panning);
        setCursor(Qt::ArrowCursor);
    }
    else if (_event->button() == Qt::LeftButton && m_state & State::SelectingCrop) {
		m_state.remove(State::SelectingCrop);
    }
    QGraphicsView::mouseReleaseEvent(_event);
}

void ot::PixmapEditWidget::resizeEvent(QResizeEvent* _event) {
    QGraphicsView::resizeEvent(_event);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

void ot::PixmapEditWidget::updateCropRect(const QPointF& _start, const QPointF& _end) {
    QRectF rect(_start, _end);
    
    rect = rect.normalized();
    
    double currentRatio = rect.width() / rect.height();
    double targetRatio = static_cast<double>(m_resultSize.width()) / m_resultSize.height();

    if (currentRatio > targetRatio) {
        // Too wide, adjust width
        double newWidth = rect.height() * targetRatio;
        double xAdjust = (rect.width() - newWidth);
        rect.setRight(rect.right() - xAdjust);
    }
    else {
        // Too tall, adjust height
        double newHeight = rect.width() / targetRatio;
        double yAdjust = (rect.height() - newHeight);
        rect.setBottom(rect.bottom() - yAdjust);
    }

    m_cropRect->setRect(rect);
}

void ot::PixmapEditWidget::updateCropRectLineWidth() {
    QTransform t = transform();
    double totalScale = std::sqrt(t.m11() * t.m11() + t.m12() * t.m12());

    QPen pen = m_cropRect->pen();
    pen.setWidthF(2. / totalScale);
    m_cropRect->setPen(pen);
}

bool ot::PixmapEditWidget::isOnCropRect(const QPointF& _scenePos) const {
	return m_cropRect->rect().contains(_scenePos);
}
