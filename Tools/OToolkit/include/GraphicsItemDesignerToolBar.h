//! @file GraphicsItemDesignerToolBar.h
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OToolkit header
#include "GraphicsItemDesigner.h"

// OpenTwin header
#include "OTCore/OTClassHelper.h"

// Qt header
#include <QtWidgets/qtoolbar.h>

// std header
#include <list>

class QGridLayout;

class GraphicsItemDesignerToolBar : public QToolBar {
	Q_OBJECT
public:
	GraphicsItemDesignerToolBar(GraphicsItemDesigner* _designer);
	virtual ~GraphicsItemDesignerToolBar();

Q_SIGNALS:
	void modeRequested(GraphicsItemDesigner::DesignerMode _mode);

	// ###########################################################################################################################################################################################################################################################################################################################

private Q_SLOTS:
	void slotLine(void);
	void slotSquare(void);
	void slotRect(void);
	void slotTriangle(void);
	void slotPolygon(void);
	void slotShape(void);

	// ###########################################################################################################################################################################################################################################################################################################################

private:
	GraphicsItemDesigner* m_designer;
};