#pragma once

// OpenTwin header
#include <openTwin/BlockEditorSharedDatatypes.h>
#include <openTwin/BlockConnector.h>

// Qt header
#include <QtCore/qstring.h>
#include <QtCore/qlist.h>
#include <QtCore/qpoint.h>
#include <QtGui/qcolor.h>

class QPainter;

namespace ot {

	class FlowBlock;
	class FlowConnection;

	enum FlowConnectorDirection {
		InputFlow,
		OutputFlow
	};

	enum FlowConnectorFigure {
		Circle,
		Triangle
	};

	class BLOCK_EDITOR_API_EXPORT FlowConnector : public BlockConnector {
	public:
		static qreal defaultHeight(void) { return 20.; };

		FlowConnector(const QString& _text, FlowConnectorDirection _direction, FlowConnectorFigure _figure);
		virtual ~FlowConnector();

		virtual void paint(QPainter* _painter, qreal _xLimit, qreal _y) override;
		virtual QPointF connectionPoint(void) const override { return m_connectionPoint; };

		// #########################################################################################################################

		// Setter / Getter

		qreal width(void) const;

		QPointF figureCenter(void) const { return m_figureCenter; };
		
		FlowConnectorDirection direction(void) const { return m_direction; };

		void setBlock(FlowBlock* _block) { m_block = _block; };
		FlowBlock* block(void) { return m_block; };
		const FlowBlock* const block(void) const { return m_block; };

		void setFigure(FlowConnectorFigure _figure) { m_figure = _figure; };
		FlowConnectorFigure figure(void) const { return m_figure; };

		void setText(const QString& _text) { m_text = _text; };
		const QString& text(void) const { return m_text; };

		void setBorderColor(const QColor& _color) { m_borderColor = _color; };
		const QColor& borderColor(void) const { return m_borderColor; };

		void setFillColor(const QColor& _color) { m_fillColor = _color; };
		const QColor& fillColor(void) const { return m_fillColor; };

		// #########################################################################################################################

		void attachToBlock(FlowBlock* _block);
		void detach(void);

		ot::FlowConnection* connectTo(FlowConnector* _connector, bool applyFillColor = true);

	private:
		void addConnection(FlowConnection* _connection);
		void removeConnection(FlowConnection* _connection);

		void drawFigure(QPainter* _painter, const QPointF& _topLeft);

		FlowBlock*				m_block;
		FlowConnectorDirection	m_direction;
		FlowConnectorFigure		m_figure;
		QString					m_text;
		QColor					m_borderColor;
		QColor					m_fillColor;
		QList<FlowConnection*>	m_connections;
		QPointF					m_figureCenter;
		QPointF					m_connectionPoint;

		FlowConnector() = delete;
		FlowConnector(FlowConnector&) = delete;
		FlowConnector& operator = (FlowConnector&) = delete;
	};

}