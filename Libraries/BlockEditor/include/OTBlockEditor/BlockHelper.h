//! @file BlockHelper.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditor/BlockEditorAPIExport.h"
#include "OTBlockEditorAPI/BlockEditorAPITypes.h"

// Qt header
#include <QtCore/qrect.h>

namespace ot {

	BLOCK_EDITOR_API_EXPORT QRectF calculateChildRect(const QRectF& _parentRect, ot::BlockComponentPosition _childPosition,const QSizeF& _childSize);

}