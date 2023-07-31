//! @file BlockGraphicsObject.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditor/BlockEditorAPIExport.h"
#include "OTBlockEditor/BlockGraphicsObject.h"
#include "OTBlockEditor/BlockPaintJob.h"
#include "OTGui/GuiTypes.h"

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

		//! @brief Will calculate and return the size of the block graphics object
		virtual QSizeF calculateSize(void) const override;

		//! @brief Will calculate and return the rectangle of the block graphics object
		//! Top->Bottom calculation (Uses calculateSize())
		virtual QRectF rect(void) const override;

		void setConnectorManger(BlockConnectorManager* _connectorManager);
		BlockConnectorManager* getConnectorManager(void) { return m_connectorManager; };

		void setLayerOrientation(ot::Orientation _orient) { m_orientation = _orient; };
		ot::Orientation layerOrientation(void) const { return m_orientation; };

		ot::DefaultBlock* getBlock(void) { return m_block; };

	private:
		ot::DefaultBlock* m_block;
		BlockConnectorManager* m_connectorManager;
		ot::Orientation m_orientation;
		BlockLayer() = delete;
		BlockLayer(const BlockLayer&) = delete;
		BlockLayer& operator = (const BlockLayer&) = delete;
	};
}