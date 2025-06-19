//! @file Painter2DEditDialogFillEntry.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/Painter2DEditDialogEntry.h"

namespace ot {

	class ColorPickButton;

	class OT_WIDGETS_API_EXPORT Painter2DEditDialogFillEntry : public Painter2DEditDialogEntry {
	public:
		Painter2DEditDialogFillEntry(const Painter2D* _painter = (Painter2D*)nullptr);
		virtual ~Painter2DEditDialogFillEntry();

		virtual DialogEntryType getEntryType() const { return Painter2DEditDialogEntry::FillType; };
		virtual QWidget* getRootWidget() const override;

		//! @brief Creates a new painter.
		//! Caller takes ownership.
		virtual Painter2D* createPainter() const override;

	private:
		ColorPickButton* m_btn;
	};

}
