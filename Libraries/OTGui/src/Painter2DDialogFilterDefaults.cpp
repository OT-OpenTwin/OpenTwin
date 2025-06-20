//! @file Painter2DDialogFilterDefaults.cpp
//! @author Alexander Kuester (alexk95)
//! @date June 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/Painter2DDialogFilterDefaults.h"

ot::Painter2DDialogFilter ot::Painter2DDialogFilterDefaults::plotCurve() {
	Painter2DDialogFilter result;

	result.setPainterTypes(Painter2DDialogFilter::Fill | ot::Painter2DDialogFilter::StyleRef);
	result.setStyleReferences(std::list<ColorStyleValueEntry> {
		ColorStyleValueEntry::PlotCurve,
		ColorStyleValueEntry::Rainbow0,
		ColorStyleValueEntry::Rainbow1,
		ColorStyleValueEntry::Rainbow2,
		ColorStyleValueEntry::Rainbow3,
		ColorStyleValueEntry::Rainbow4,
		ColorStyleValueEntry::Rainbow5,
		ColorStyleValueEntry::Rainbow6,
		ColorStyleValueEntry::Rainbow7,
		ColorStyleValueEntry::Rainbow8,
		ColorStyleValueEntry::Rainbow9,
	});

	return result;
}
