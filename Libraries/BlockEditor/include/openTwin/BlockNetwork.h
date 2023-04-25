#pragma once

// OpenTwin header
#include <openTwin/BlockEditorSharedDatatypes.h>

// Qt header
#include <QtCore/qlist.h>
#include <QtWidgets/qgraphicsscene.h>

class QGraphicsSceneMouseEvent;

namespace ot {

	class Block;
	class BlockConnection;

	class BLOCK_EDITOR_API_EXPORT BlockNetwork : public QGraphicsScene {
		Q_OBJECT
	public:
		BlockNetwork();
		virtual ~BlockNetwork();

		void setGridSize(int _size) { m_gridSize = _size; };
		int gridSize(void) const { return m_gridSize; };

		void addBlock(Block* _block);
		void addBlock(Block* _block, const QPoint& _pos);

		void registerConnection(BlockConnection* _connection);

	signals:
		void backgroundDoubleClicked(void);
		void itemDoubleClicked(Block* _block);
		
	protected:
		virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* _event) override;

		virtual void drawBackground(QPainter* _painter, const QRectF& _rect) override;

	private:
		int						m_gridSize;
		QList<Block*>			m_blocks;
		QList<BlockConnection*>	m_connections;
		BlockConnection*		m_selectedConnection;
	};

}