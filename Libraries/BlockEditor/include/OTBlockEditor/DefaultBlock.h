//! @file DefaultBlock.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditor/Block.h"

namespace ot {

	class BLOCK_EDITOR_API_EXPORT DefaultBlock : public ot::Block {
		Q_OBJECT
	public:
		DefaultBlock();
		virtual ~DefaultBlock();

		virtual qreal blockWidth(void) const override;
		virtual qreal blockHeigth(void) const override;
	};
}