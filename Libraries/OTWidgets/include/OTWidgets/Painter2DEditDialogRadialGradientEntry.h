// @otlicense

//! @file Painter2DEditDialogRadialGradientEntry.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/Painter2DEditDialogEntry.h"

class QWidget;
class QVBoxLayout;

namespace ot {

	class CheckBox;
	class DoubleSpinBox;
	class Painter2DEditDialogGradientBase;

	class OT_WIDGETS_API_EXPORT Painter2DEditDialogRadialGradientEntry : public Painter2DEditDialogEntry {
		Q_OBJECT
	public:
		Painter2DEditDialogRadialGradientEntry(const Painter2D* _painter = (Painter2D*)nullptr);
		virtual ~Painter2DEditDialogRadialGradientEntry();

		virtual DialogEntryType getEntryType() const { return Painter2DEditDialogEntry::RadialType; };
		virtual QWidget* getRootWidget() const override { return m_cLayW; };

		//! @brief Creates a new painter.
		//! Caller takes ownership.
		virtual Painter2D* createPainter() const override;

	private Q_SLOTS:
		void slotFocalEnabledChanged();

	private:
		QWidget* m_cLayW;
		QVBoxLayout* m_cLay;
		Painter2DEditDialogGradientBase* m_gradientBase;

		DoubleSpinBox* m_centerX;
		DoubleSpinBox* m_centerY;
		DoubleSpinBox* m_centerRadius;
		CheckBox* m_useFocal;
		DoubleSpinBox* m_focalX;
		DoubleSpinBox* m_focalY;
		DoubleSpinBox* m_focalRadius;
	};

}
