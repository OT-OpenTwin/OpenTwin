//! @file Painter2DEditDialogLinearGradientEntry.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/Painter2DEditDialogEntry.h"

class QWidget;
class QVBoxLayout;

namespace ot {

	class DoubleSpinBox;
	class Painter2DEditDialogGradientBase;

	class OT_WIDGETS_API_EXPORT Painter2DEditDialogLinearGradientEntry : public Painter2DEditDialogEntry {
	public:
		Painter2DEditDialogLinearGradientEntry(const Painter2D* _painter = (Painter2D*)nullptr);
		virtual ~Painter2DEditDialogLinearGradientEntry();

		virtual DialogEntryType getEntryType() const { return Painter2DEditDialogEntry::LinearType; };
		virtual QWidget* getRootWidget() const override { return m_cLayW; };

		//! @brief Creates a new painter.
		//! Caller takes ownership.
		virtual Painter2D* createPainter() const override;

	private:
		QWidget* m_cLayW;
		QVBoxLayout* m_cLay;
		Painter2DEditDialogGradientBase* m_gradientBase;

		DoubleSpinBox* m_startX;
		DoubleSpinBox* m_startY;
		DoubleSpinBox* m_finalX;
		DoubleSpinBox* m_finalY;
	};

}