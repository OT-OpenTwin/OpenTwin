#pragma once

// OpenTwin header
#include <openTwin/BlockEditorSharedDatatypes.h>
#include <openTwin/Block.h>

// Qt header
#include <QtCore/qlist.h>
#include <QtCore/qstring.h>
#include <QtGui/qpixmap.h>

namespace ot {

	class FlowConnector;

	class BLOCK_EDITOR_API_EXPORT FlowBlock : public Block {
		Q_OBJECT
	public:
		FlowBlock(const QString& _title = QString());
		virtual ~FlowBlock();

		virtual qreal blockWidth(void) const override;
		virtual qreal blockHeigth(void) const override;

		virtual BlockConnector* connectorAt(const QPointF& _point) const override;

		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget = nullptr) override;

		void setTitle(const QString& _title) { m_title = _title; };
		const QString& title(void) const { return m_title; };

		void setTitleColor(const QColor& _color) { m_titleColor = _color; };
		const QColor& titleColor(void) const { return m_titleColor; };

		void addConnector(FlowConnector* _connector);
		void removeConnector(FlowConnector* _connector);

		void setTitleImage(const QPixmap& _Image);
		QPixmap titleImage(void) const;

		void setBottomLeftImage(const QPixmap& _Image);
		QPixmap bottomLeftImage(void) const;

		void setBottomRightImage(const QPixmap& _Image);
		QPixmap bottomRightImage(void) const;

	private:
		QList<FlowConnector*>	m_inputConnectors;
		QList<FlowConnector*>	m_outputConnectors;
		QString					m_title;
		QColor					m_titleColor;

		QPixmap*				m_titleImage;
		QPixmap*				m_bottomLeftImage;
		QPixmap*				m_bottomRightImage;
	};

}