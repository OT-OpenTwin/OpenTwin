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

		QSizeF fitIntoLimits(const QSizeF& _size) const;

	private:
		QMarginsF        m_margins;
		LengthLimitation m_heightLimit;
		LengthLimitation m_widthLimit;
		
	};

}