//! @file Painter2DEditDialogReferenceEntry.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/Painter2DEditDialogEntry.h"

namespace ot {

	class ComboBox;

	class OT_WIDGETS_API_EXPORT Painter2DEditDialogReferenceEntry : public Painter2DEditDialogEntry {
	public:
		Painter2DEditDialogReferenceEntry(const Painter2D* _painter = (Painter2D*)nullptr);
		virtual ~Painter2DEditDialogReferenceEntry();

		virtual DialogEntryType getEntryType() const { return Painter2DEditDialogEntry::ReferenceType; };
		virtual QWidget* getRootWidget() const override;

		//! @brief Creates a new painter.
		//! Caller takes ownership.
		virtual Painter2D* createPainter() const override;

	private:
		ComboBox* m_comboBox;
	};

}
