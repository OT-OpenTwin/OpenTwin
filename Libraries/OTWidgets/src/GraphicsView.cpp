//! @file GraphicsView.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/GraphicsItemCfg.h"
#include "OTWidgets/GraphicsView.h"
#include "OTWidgets/GraphicsItem.h"
#include "OTWidgets/GraphicsFactory.h"

// Qt header
#include <QtGui/qevent.h>
#include <QtCore/qmimedata.h>
//#include <QtWidgets/qgraphicsscene.h>

//! If this is true, the graphics view will force a minimum size
#define OT_CFG_ForceMinimumGraphicsViewSize true

#if (OT_CFG_ForceMinimumGraphicsViewSize == true)
// Only to use in this context
#define OT_INTERN_MinimumGraphicsViewSizeWidth 500
// Only to use in this context
#define OT_INTERN_MinimumGraphicsViewSizeHeight 500

//! @brief Adjust the provided variables according to the GraphicsViews default forced minimum size (if enabled)
//! @param ___w Width variable
//! @param ___h Height variable
#define OT_AdjustMinimumGraphicsViewSize(___w, ___h) if (___w < OT_INTERN_MinimumGraphicsViewSizeWidth) { ___w = OT_INTERN_MinimumGraphicsViewSizeWidth; }; if (___h < OT_INTERN_MinimumGraphicsViewSizeHeight) { ___h = OT_INTERN_MinimumGraphicsViewSizeHeight; }

#else
//! @brief Adjust the provided variables according to the GraphicsViews default forced minimum size (if enabled)
//! @param ___w Width variable
//! @param ___h Height variable
#define OT_AdjustMinimumGraphicsViewSize(___w, ___h)
#endif
ot::GraphicsView::GraphicsView() : m_isPressed(false), m_wheelEnabled(true) {
	setDragMode(QGraphicsView::DragMode::ScrollHandDrag);
}

ot::GraphicsView::~GraphicsView() {

}

// ########################################################################################################

void ot::GraphicsView::resetView(void) {
	QGraphicsScene* s = scene();
	if (s == nullptr) return;
	QRectF boundingRect = s->itemsBoundingRect();
	setSceneRect(QRectF());
	int w = boundingRect.width();
	int h = boundingRect.height();
	OT_AdjustMinimumGraphicsViewSize(w, h);
	QRectF viewRect = boundingRect.marginsAdded(QMarginsF(w, h, w, h));
	fitInView(viewRect, Qt::AspectRatioMode::KeepAspectRatio);
	centerOn(viewRect.center());
}

void ot::GraphicsView::viewAll(void) {
	QGraphicsScene* s = scene();
	if (s == nullptr) return;
	QRectF boundingRect = mapFromScene(s->itemsBoundingRect()).boundingRect();
	int w = boundingRect.width();
	int h = boundingRect.height();
	OT_AdjustMinimumGraphicsViewSize(w, h);
	QRect viewPortRect = viewport()->rect().marginsRemoved(QMargins(w, h, w, h));

	if (viewPortRect.width() > boundingRect.width() && viewPortRect.height() > boundingRect.height())
	{
		resetView();
	}
}

// ########################################################################################################

// Protected: Slots

void ot::GraphicsView::wheelEvent(QWheelEvent* _event)
{
	if (!m_wheelEnabled) return;
	const ViewportAnchor anchor = transformationAnchor();
	setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	int angle = _event->angleDelta().y();
	qreal factor;
	if (angle > 0) {
		factor = 1.1;
	}
	else {
		factor = 0.9;
	}
	scale(factor, factor);
	update();

	setTransformationAnchor(anchor);

	viewAll();
}

void ot::GraphicsView::enterEvent(QEvent* _event)
{
	QGraphicsView::enterEvent(_event);
	viewport()->setCursor(Qt::CrossCursor);
}

void ot::GraphicsView::mousePressEvent(QMouseEvent* _event)
{
	QGraphicsView::mousePressEvent(_event);
	viewport()->setCursor(Qt::CrossCursor);

	if (_event->button() == Qt::LeftButton) {
		m_isPressed = true;
	}
}

void ot::GraphicsView::mouseReleaseEvent(QMouseEvent* _event)
{
	QGraphicsView::mouseReleaseEvent(_event);
	viewport()->setCursor(Qt::CrossCursor);

	m_isPressed = false;
}

void ot::GraphicsView::mouseMoveEvent(QMouseEvent* _event)
{
	QGraphicsView::mouseMoveEvent(_event);
	if (m_isPressed) { mousePressedMoveEvent(_event); }
}

void ot::GraphicsView::keyPressEvent(QKeyEvent* _event)
{
	if (_event->key() == Qt::Key_Space)
	{
		// Reset the view
		resetView();
	}
}

void ot::GraphicsView::keyReleaseEvent(QKeyEvent* _event) {}

void ot::GraphicsView::resizeEvent(QResizeEvent* _event)
{
	QGraphicsView::resizeEvent(_event);

	viewAll();
}

void ot::GraphicsView::dragEnterEvent(QDragEnterEvent* _event) {
	// Check if the events mime data contains the configuration
	if (!_event->mimeData()->data(OT_GRAPHICSITEM_MIMETYPE_Configuration).isEmpty()) {
		_event->acceptProposedAction();
	}
	else {
		GraphicsView::dragEnterEvent(_event);
	}
}

void ot::GraphicsView::dropEvent(QDropEvent* _event) {
	QByteArray cfgRaw = _event->mimeData()->data(OT_GRAPHICSITEM_MIMETYPE_Configuration);
	if (cfgRaw.isEmpty()) {
		OT_LOG_W("Drop event reqected: MimeData not matching");
		return;
	}

	// Generate configuration from raw data
	ot::GraphicsItemCfg* cfg = nullptr;
	try {
		OT_rJSON_parseDOC(cfgDoc, cfgRaw.toStdString().c_str());
		OT_rJSON_val cfgObj = cfgDoc.GetObject();
		cfg = ot::SimpleFactory::instance().createType<ot::GraphicsItemCfg>(cfgObj);
	}
	catch (const std::exception& e) {
		OT_LOG_EAS(e.what());
		delete cfg;
		return;
	}
	catch (...) {
		OT_LOG_EA("Unknown error");
		delete cfg;
		return;
	}

	if (cfg == nullptr) {
		OT_LOG_WA("No config created");
		delete cfg;
		return;
	}

	// Store current event position to position the new item at this pos
	QPointF position = this->mapToScene(mapToGlobal(_event->pos()));

	// Create graphics item from configuration
	ot::GraphicsItem* newItem = nullptr;
	try {
		newItem = ot::GraphicsFactory::itemFromConfig(cfg);
		newItem->setGraphicsItemFlags(ot::GraphicsItem::ItemNetworkContext);
		newItem->finalizeAsRootItem(scene());
	}
	catch (const std::exception& _e) {
		OT_LOG_EAS(_e.what());
		delete newItem;
		delete cfg;
		return;
	}
	catch (...) {
		OT_LOG_EA("Unknown error occured");
		delete newItem;
		delete cfg;
		return;
	}

	//newBlock->moveBy(position.x(), position.y());

	delete cfg;

	_event->acceptProposedAction();
}

void ot::GraphicsView::dragMoveEvent(QDragMoveEvent* _event) {
	// Check if the events mime data contains the configuration
	if (!_event->mimeData()->data(OT_GRAPHICSITEM_MIMETYPE_Configuration).isEmpty()) {
		_event->acceptProposedAction();
		OT_LOG_D("Drag move event accepted for: Block");
	}
	else {
		GraphicsView::dragMoveEvent(_event);
	}
}