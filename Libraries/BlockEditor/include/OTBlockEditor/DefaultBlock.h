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
		DefaultBlock(BlockGraphicsItemGroup* _graphicsItemGroup);
		virtual ~DefaultBlock();

		virtual qreal blockWidth(void) const override;
		virtual qreal blockHeigth(void) const override;

		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget = (QWidget*)nullptr) override;

		//! @brief Add the provided layer at the top of the block
		//! Block takes ownership of the layer
		void addLayer(BlockLayer* _layer);

		const std::list<BlockLayer*>& layers(void) const { return m_layers; };

	protected:
		virtual void attachChildsToGroup(BlockGraphicsItemGroup* _gig) override;

	private:
		std::list<BlockLayer*> m_layers;

		DefaultBlock() = delete;
		DefaultBlock(const DefaultBlock&) = delete;
		DefaultBlock& operator = (const DefaultBlock&) = delete;
	};
}