//! @file BlockGraphicsObject.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditor/BlockEditorAPIExport.h"
#include "OTBlockEditor/BlockGraphicsObject.h"
#include "OTBlockEditor/BlockPaintJob.h"

// Qt header
#include <QtCore/qrect.h>

class QWidget;
class QPainter;
class QStyleOptionGraphicsItem;

namespace ot {

	class DefaultBlock;
	class BlockConnectorManager;

	class BLOCK_EDITOR_API_EXPORT BlockLayer : public ot::BlockPaintJob, public ot::BlockGraphicsObject {
	public:
		BlockLayer(ot::DefaultBlock * _block, BlockConnectorManager * _connectorManager = (BlockConnectorManager *)nullptr);
		virtual ~BlockLayer();

		virtual QueueResultFlags runPaintJob(AbstractQueue* _queue, BlockPaintJobArg* _arg) override;
		virtual void paintLayer(const QRectF& _rect, QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget = (QWidget*)nullptr) = 0;

		//! @brief Calculate the ideal size for this layer.
		//! The block will add the stretching factor if the block is bigger than the layer
		virtual QSizeF layerSizeHint(void) const;

		void setConnectorManger(BlockConnectorManager* _connectorManager);
		BlockConnectorManager* getConnectorManager(void) { return m_connectorManager; };

	private:
		ot::DefaultBlock* m_block;
		BlockConnectorManager* m_connectorManager;

		BlockLayer() = delete;
		BlockLayer(const BlockLayer&) = delete;
		BlockLayer& operator = (const BlockLayer&) = delete;
	};
}