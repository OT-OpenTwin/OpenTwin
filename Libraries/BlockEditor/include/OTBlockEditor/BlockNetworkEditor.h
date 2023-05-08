//! @file BlockNetworkEditor.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditor/BlockEditorAPIExport.h"
#include "OTWidgets/GraphicsView.h"

// Qt header
#include <QtCore/qobject.h>

namespace ot {

	class Block;
	class BlockNetwork;

	class BLOCK_EDITOR_API_EXPORT BlockNetworkEditor : public GraphicsView {
		Q_OBJECT
	public:
		BlockNetworkEditor();
		virtual ~BlockNetworkEditor();

		BlockNetwork* network(void) { return m_network; };

	protected:
		virtual void mousePressedMoveEvent(QMouseEvent* _event) override;

	private:
		BlockNetwork* m_network;
	};

}