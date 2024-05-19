//! @file GraphicsItemDesignerView.h
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/GraphicsView.h"

class GraphicsItemDesignerScene;
namespace ot { class GraphicsEllipseItem; }

class GraphicsItemDesignerView : public ot::GraphicsView {
	Q_OBJECT
public:
	enum Mode {
		NoMode,
		PointPickingMode
	};

	GraphicsItemDesignerView();
	virtual ~GraphicsItemDesignerView();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

	void enablePickingMode(void);
	void disablePickingMode(void);

	// ###########################################################################################################################################################################################################################################################################################################################

Q_SIGNALS:
	void pointSelected(const QPointF& _pt);
	void cancelRequested(void);

private:
	friend class GraphicsItemDesignerScene;

	void emitPointSelected(const QPointF& _pt);
	void emitCancelRequest(void);

	GraphicsItemDesignerScene* m_scene;
	ot::GraphicsEllipseItem* m_cursorItem;
	Mode m_mode;
};