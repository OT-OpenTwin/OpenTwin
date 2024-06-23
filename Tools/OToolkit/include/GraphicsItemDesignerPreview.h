//! @file GraphicsItemDesignerPreview.h
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/GraphicsView.h"

namespace ot { class GraphicsItem; };

class GraphicsItemDesignerPreview : public ot::GraphicsView {
public:
	GraphicsItemDesignerPreview();
	virtual ~GraphicsItemDesignerPreview();

	//! \brief Updates the current preview.
	void updateCurrentItem(const ot::GraphicsItemCfg* _newConfiguration);

private:
	ot::GraphicsItem* m_item;

};
