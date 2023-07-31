//! @file BlockGraphicsObject.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditor/BlockEditorAPIExport.h"
#include "OTGui/LengthLimitation.h"
#include "OTGui/Margins.h"

// Qt header
#include <QtCore/qsize.h>
#include <QtCore/qmargins.h>
#include <QtCore/qrect.h>

namespace ot {

	class BLOCK_EDITOR_API_EXPORT BlockGraphicsObject {
	public:
		BlockGraphicsObject() {};
		virtual ~BlockGraphicsObject() {};

		void setMargins(const ot::MarginsD& _margins);
		void setMargins(const QMarginsF& _margins) { m_margins = _margins; };
		const QMarginsF& margins(void) const { return m_margins; };

		void setHeightLimit(const LengthLimitation& _limit) { m_heightLimit = _limit; };
		LengthLimitation& getHeightLimit(void) { return m_heightLimit; };
		const LengthLimitation& heightLimit(void) const { return m_heightLimit; };

		void setWidthLimit(const LengthLimitation& _limit) { m_widthLimit = _limit; };
		LengthLimitation& getWidthLimit(void) { return m_widthLimit; };
		const LengthLimitation& widthLimit(void) const { return m_widthLimit; };

		//! @brief Will calculate and return the rectangle of the block graphics object
		//! Top->Bottom calculation (Uses calculateSize())
		virtual QRectF rect(void) const = 0;

		//! @brief Will apply the currently set LengthLimitation on the provided size
		//! @param _size The size to adjust
		QSizeF applyLimits(const QSizeF& _size) const;

		//! @brief Will calculate and return the size of the block graphics object
		//! Bottom->Top calculation
		virtual QSizeF calculateSize(void) const = 0;

		//! @brief Will add the currently set margins to the provided size
		//! @param _size The size increase
		QSizeF addMargins(const QSizeF& _size) const;

	private:
		QMarginsF        m_margins;
		LengthLimitation m_heightLimit;
		LengthLimitation m_widthLimit;
	};

}