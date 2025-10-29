// @otlicense

//! @file GraphicsItemDesignerDrawHandler.h
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"

// Qt header
#include <QtCore/qobject.h>

class GraphicsItemDesignerView;
class GraphicsItemDesignerItemBase;
class GraphicsItemDesignerInfoOverlay;

class GraphicsItemDesignerDrawHandler : public QObject {
	Q_OBJECT
public:
	enum DrawMode {
		NoMode,
		Line,
		Square,
		Rect,
		Circle,
		Ellipse,
		Arc,
		Triangle,
		Polygon,
		Shape,
		Text
	};

	GraphicsItemDesignerDrawHandler(GraphicsItemDesignerView* _view);
	virtual ~GraphicsItemDesignerDrawHandler();

	void startDraw(DrawMode _mode);

	//! \brief Cancels the current draw.
	//! The item will be removed.
	void cancelDraw(void);

	//! \brief Stops the current draw and returns the drawn item if valid.
	//! The current preview item will be returned and forgotten by the handler.
	//! The caller takes ownership of the item.
	//! The returned item is still placed in the scene.
	GraphicsItemDesignerItemBase* stopDraw(void);

	//! \brief Will check if the current draw is valid and emits drawCompleted if valid otherwise removes the item.
	//! The last control point of the item will be removed.
	void checkStopDraw(void);

	void updatePosition(const QPointF& _pos);
	void positionSelected(const QPointF& _pos);

	//! \brief Generates the next available UID
	ot::UID generateUid(void) { return ++m_currentUid; };

	//! \brief Resets the UID to the initial state.
	void resetUid(void) { m_currentUid = 0; };

Q_SIGNALS:
	void drawCompleted();
	void drawCancelled();

private:
	QPointF constainPosition(const QPointF& _pos) const;
	QString modeString(void);
	void createPreviewItem(void);

	ot::UID m_currentUid;

	DrawMode m_mode;
	GraphicsItemDesignerView* m_view;
	GraphicsItemDesignerInfoOverlay* m_overlay;
	GraphicsItemDesignerItemBase* m_previewItem;
};