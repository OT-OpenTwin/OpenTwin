//! @file BlockLayer.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditor/BlockEditorAPIExport.h"
#include "OTGui/Margins.h"

// Qt header
#include <QtCore/qrect.h>
#include <QtCore/qmargins.h>

class QWidget;
class QPainter;
class QStyleOptionGraphicsItem;

namespace ot {

	class DefaultBlock;

	class BLOCK_EDITOR_API_EXPORT BlockLayer {
	public:
		BlockLayer(ot::DefaultBlock * _block);
		virtual ~BlockLayer();

		virtual void paintLayer(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget = (QWidget*)nullptr) = 0;

		//! @brief Returns the rect for this layer for painting
		virtual QRectF layerRect(void) const;

		void setMargins(const ot::MarginsD& _margins);
		void setMargins(const QMarginsF& _margins) { m_margins = _margins; };
		const QMarginsF& margins(void) const { return m_margins; };

	private:
		ot::DefaultBlock* m_block;
		QMarginsF         m_margins;

		BlockLayer() = delete;
		BlockLayer(const BlockLayer&) = delete;
		BlockLayer& operator = (const BlockLayer&) = delete;
	};
}