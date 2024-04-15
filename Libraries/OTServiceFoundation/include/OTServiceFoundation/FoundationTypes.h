#pragma once

#include "OTCore/Flags.h"

namespace ot {
	enum itemFlag {
		NoFlags =				0x0,
		ItemIsCheckable =		0x1,
		ItemIsVisible =			0x2,
		itemFlagMask =			0x3
	};
	typedef Flags<itemFlag> itemFlags;

	enum itemCheckedState {
		NoCheckedState =		0x0,
		Checked =				0x1,
		Unchecked =				0x2,
		PartiallyChecked =		0x4,
		itemCheckedStateMask =	0x7
	};

	typedef Flags<itemCheckedState> itemCheckedStateFlags;
}

OT_ADD_FLAG_FUNCTIONS(ot::itemFlag)
OT_ADD_FLAG_FUNCTIONS(ot::itemCheckedState)

