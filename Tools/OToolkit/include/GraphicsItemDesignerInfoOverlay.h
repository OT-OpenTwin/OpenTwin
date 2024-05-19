//! @file GraphicsItemDesignerInfoOverlay.h
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OverlayWidgetBase.h"

// Qt header
#include <QtCore/qstring.h>

class QLabel;
class GraphicsItemDesigner;

class GraphicsItemDesignerInfoOverlay : public ot::OverlayWidgetBase {
public:
	GraphicsItemDesignerInfoOverlay(const QString& _text, GraphicsItemDesigner* _designer);

	void setText(const QString& _text);

private:
	QLabel* m_label;

};
