//! @file BlockLayer.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditor/BlockEditorAPIExport.h"
#include "OTBlockEditor/BlockPaintJob.h"
#include "OTGui/Margins.h"
#include "OTGui/LengthLimitation.h"

// Qt header
#include <QtCore/qrect.h>
#include <QtCore/qmargins.h>

class QWidget;
class QPainter;
class QStyleOptionGraphicsItem;

namespace ot {

	class DefaultBlock;
	class BlockConnectorManager;

	class BLOCK_EDITOR_API_EXPORT BlockLayer : public ot::BlockPaintJob {
	public:
		BlockLayer(ot::DefaultBlock * _block, BlockConnectorManager * _connectorManager = (BlockConnectorManager *)nullptr);
		virtual ~BlockLayer();

		virtual QueueResultFlags runPaintJob(AbstractQueue* _queue, BlockPaintJobArg* _arg) override;
		virtual void paintLayer(const QRectF& _rect, QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget = (QWidget*)nullptr) = 0;

		//! @brief Calculate the ideal size for this layer.
		//! The block will add the stretching factor if the block is bigger than the layer
		virtual QSizeF layerSizeHint(void) const;

		void setMargins(const ot::MarginsD& _margins);
		void setMargins(const QMarginsF& _margins) { m_margins = _margins; };
		const QMarginsF& margins(void) const { return m_margins; };

		void setConnectorManger(BlockConnectorManager* _connectorManager);
		BlockConnectorManager* getConnectorManager(void) { return m_connectorManager; };

		void setHeightLimit(const LengthLimitation& _limit) { m_heightLimit = _limit; };
		LengthLimitation& getHeightLimit(void) { return m_heightLimit; };
		const LengthLimitation& heightLimit(void) const { return m_heightLimit; };
		
		void setWidthLimit(const LengthLimitation& _limit) { m_widthLimit = _limit; };
		LengthLimitation& getWidthLimit(void) { return m_widthLimit; };
		const LengthLimitation& widthLimit(void) const { return m_widthLimit; };

	protected:
		void adjustSizeToLimits(QSizeF& _size) const;

	private:
		ot::DefaultBlock* m_block;
		QMarginsF         m_margins;
		BlockConnectorManager* m_connectorManager;
		LengthLimitation m_heightLimit;
		LengthLimitation m_widthLimit;

		BlockLayer() = delete;
		BlockLayer(const BlockLayer&) = delete;
		BlockLayer& operator = (const BlockLayer&) = delete;
	};
}