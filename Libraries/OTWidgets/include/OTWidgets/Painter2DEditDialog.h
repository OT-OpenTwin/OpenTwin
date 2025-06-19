//! @file Painter2DEditDialog.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/GuiTypes.h"
#include "OTGui/GradientPainterStop2D.h"
#include "OTWidgets/Dialog.h"
#include "OTWidgets/ColorStyle.h"

class QVBoxLayout;

namespace ot {

	class ComboBox;
	class Painter2D;
	class PushButton;
	class Painter2DPreview;
	class Painter2DEditDialogEntry;

	class OT_WIDGETS_API_EXPORT Painter2DEditDialog : public Dialog {
		Q_OBJECT
	public:
		enum PainterEntryType {
			FillType,
			LinearType,
			RadialType
		};

		Painter2DEditDialog(const Painter2D* _painter = (Painter2D*)nullptr);
		virtual ~Painter2DEditDialog();

		const Painter2D* currentPainter() const { return m_painter; };
		Painter2D* createPainter() const;

		void setHasChanged(bool _changed) { m_changed = _changed; };
		bool hasChanged() const { return m_changed; };

	Q_SIGNALS:
		void painterChanged();

	private Q_SLOTS:
		void slotTypeChanged();
		void slotUpdate();
		void slotConfirm();

	private:
		void ini();
		void applyPainter(const Painter2D* _painter);

		Painter2D* m_painter;

		bool m_changed;
		ComboBox* m_typeSelectionBox;
		QVBoxLayout* m_vLayout;
		Painter2DPreview* m_preview;
		Painter2DEditDialogEntry* m_currentEntry;
		PushButton* m_confirm;
		PushButton* m_cancel;
		QWidget* m_stretchItem;
	};

}
