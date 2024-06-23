//! @file GraphicsItemDesignerView.h
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/GraphicsView.h"

class GraphicsItemDesigner;
class GraphicsItemDesignerScene;
class GraphicsItemDesignerDrawHandler;
class GraphicsItemDesignerViewStatusOverlay;
namespace ot { class GraphicsEllipseItem; }

class GraphicsItemDesignerView : public ot::GraphicsView {
	Q_OBJECT
	OT_DECL_NODEFAULT(GraphicsItemDesignerView)
	OT_DECL_NOCOPY(GraphicsItemDesignerView)
public:
	GraphicsItemDesignerView(GraphicsItemDesigner* _designer);
	virtual ~GraphicsItemDesignerView();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

	void enablePickingMode(void);
	void disablePickingMode(void);

	void setItemSize(const QSizeF& _size);
	const QSizeF& getItemSize(void) const;

	void setDrawHandler(GraphicsItemDesignerDrawHandler* _handler) { m_drawHandler = _handler; };
	GraphicsItemDesignerDrawHandler* getDrawHandler(void) const { return m_drawHandler; };

	GraphicsItemDesignerScene* getDesignerScene(void) const { return m_scene; };

	void setSelectionChangeInProgress(bool _active) { m_selectionChangeInProgress = _active; };
	bool getSelectionChangeInProgress(void) const { return m_selectionChangeInProgress; };

	// ###########################################################################################################################################################################################################################################################################################################################

protected:
	virtual void keyPressEvent(QKeyEvent* _event) override;
	
private Q_SLOTS:
	void slotSceneSelectionChanged(void);

private:
	friend class GraphicsItemDesignerScene;

	void fwdPointSelected(const QPointF& _pt);
	void fwdCancelRequest(void);
	void fwdPositionChanged(const QPointF& _pt);

	bool m_selectionChangeInProgress;
	GraphicsItemDesigner* m_designer;
	GraphicsItemDesignerViewStatusOverlay* m_infoOverlay;
	GraphicsItemDesignerScene* m_scene;
	GraphicsItemDesignerDrawHandler* m_drawHandler;
	ot::GraphicsEllipseItem* m_cursorItem;

};