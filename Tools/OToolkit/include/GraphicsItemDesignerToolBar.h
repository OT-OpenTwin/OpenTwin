//! @file GraphicsItemDesignerToolBar.h
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OToolkit header
#include "GraphicsItemDesignerDrawHandler.h"

// OpenTwin header
#include "OTCore/OTClassHelper.h"

// Qt header
#include <QtWidgets/qtoolbar.h>

// std header
#include <list>

class QGridLayout;
class GraphicsItemDesigner;

class GraphicsItemDesignerToolBar : public QToolBar {
	Q_OBJECT
public:
	GraphicsItemDesignerToolBar(GraphicsItemDesigner* _designer);
	virtual ~GraphicsItemDesignerToolBar();

Q_SIGNALS:
	void modeRequested(GraphicsItemDesignerDrawHandler::DrawMode _mode);

	// ###########################################################################################################################################################################################################################################################################################################################

private Q_SLOTS:
	void slotLine(void);
	void slotSquare(void);
	void slotRect(void);
	void slotCircle(void);
	void slotEllipse(void);
	void slotTriangle(void);
	void slotPolygon(void);
	void slotShape(void);

	// ###########################################################################################################################################################################################################################################################################################################################

private:
	GraphicsItemDesigner* m_designer;
};