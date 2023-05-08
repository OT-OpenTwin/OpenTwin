//! @file BlockNetwork.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditor/BlockEditorAPIExport.h"
#include "OTWidgets/GraphicsScene.h"

// Qt header
#include <QtCore/qlist.h>

class QGraphicsSceneMouseEvent;

namespace ot {

	class Block;
	class BlockConnection;

	class BLOCK_EDITOR_API_EXPORT BlockNetwork : public GraphicsScene {
		Q_OBJECT
	public:
		BlockNetwork();
		virtual ~BlockNetwork();

		void addBlock(Block* _block);
		void addBlock(Block* _block, const QPoint& _pos);

		void registerConnection(BlockConnection* _connection);

	signals:
		void backgroundDoubleClicked(void);
		void itemDoubleClicked(Block* _block);

	protected:
		virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* _event) override;

	private:
		QList<Block*>			m_blocks;
		QList<BlockConnection*>	m_connections;
		BlockConnection*		m_selectedConnection;
	};

}