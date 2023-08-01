//! @file DefaultBlock.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditor/Block.h"

// std header
#include <list>

namespace ot {

	class BlockLayer;

	class BLOCK_EDITOR_API_EXPORT DefaultBlock : public ot::Block {
		Q_OBJECT
	public:
		DefaultBlock();
		virtual ~DefaultBlock();

		//! @brief Will calculate and return the size of the block graphics object
		virtual QSizeF calculateSize(void) const override;

		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget = (QWidget*)nullptr) override;

		virtual QSizeF sizeHint(Qt::SizeHint _which, const QSizeF& _constraint) const override;

		//! @brief Add the provided layer at the top of the block
		//! Block takes ownership of the layer
		void addLayer(BlockLayer* _layer);

		const std::list<BlockLayer*>& layers(void) const { return m_layers; };

		virtual void finalize(void) override;

	protected:
		virtual void placeChildsOnScene(QGraphicsScene* _scene) override;

		virtual void moveChildsBy(const QPointF& _delta) override;

	private:
		std::list<BlockLayer*> m_layers;

		DefaultBlock(const DefaultBlock&) = delete;
		DefaultBlock& operator = (const DefaultBlock&) = delete;
	};
}