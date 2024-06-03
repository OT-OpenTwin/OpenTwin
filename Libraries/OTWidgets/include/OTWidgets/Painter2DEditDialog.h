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

class QLabel;
class QGroupBox;
class QVBoxLayout;
class QHBoxLayout;

namespace ot {

	class SpinBox;
	class CheckBox;
	class ComboBox;
	class Painter2D;
	class PushButton;
	class DoubleSpinBox;
	class ColorPickButton;
	class Painter2DPreview;
	class GradientPainter2D;

	namespace intern {
		class OT_WIDGETS_API_EXPORT Painter2DEditDialogGradientBase : public QObject {
			Q_OBJECT
		public:
			Painter2DEditDialogGradientBase(QVBoxLayout* _layout, const GradientPainter2D* _painter);
			virtual ~Painter2DEditDialogGradientBase();

			inline void addStop(double _pos, const ot::Color& _color) { this->addStop(GradientPainterStop2D(_pos, _color)); };
			void addStop(const GradientPainterStop2D& _stop);
			void addStops(const std::vector<GradientPainterStop2D>& _stops);
			std::vector<GradientPainterStop2D>stops(void) const;
			ot::GradientSpread gradientSpread(void);

		Q_SIGNALS:
			void valuesChanged(void);

		private Q_SLOTS:
			void slotCountChanged(int _newCount);
			void slotValueChanged(void);

		private:
			struct StopEntry {
				QGroupBox* box;
				DoubleSpinBox* pos;
				ColorPickButton* color;
			};
			void rebuild(const std::vector<GradientPainterStop2D>& _stops);
			StopEntry createStopEntry(size_t stopIx, const GradientPainterStop2D& _stop = GradientPainterStop2D()) const;
			ot::GradientPainterStop2D createStopFromEntry(const StopEntry& _entry) const;

			QVBoxLayout* m_layout;
			ComboBox* m_spreadBox;

			SpinBox* m_stopsBox;
			std::list<StopEntry> m_stops;
		};
	}

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT Painter2DEditDialogEntry : public QObject {
		Q_OBJECT
	public:
		enum DialogEntryType {
			FillType,
			LinearType,
			RadialType,
			ReferenceType
		};

		Painter2DEditDialogEntry() {};
		virtual ~Painter2DEditDialogEntry();

		virtual DialogEntryType getEntryType(void) const = 0;
		virtual QWidget* getRootWidget(void) const = 0;

		//! @brief Creates a new painter.
		//! Caller takes ownership.
		virtual Painter2D* createPainter(void) const = 0;

	Q_SIGNALS:
		void valueChanged(void);

	public Q_SLOTS:
		void slotValueChanged(void);
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT Painter2DEditDialogFillEntry : public Painter2DEditDialogEntry {
	public:
		Painter2DEditDialogFillEntry(const Painter2D* _painter = (Painter2D*)nullptr);
		virtual ~Painter2DEditDialogFillEntry();

		virtual DialogEntryType getEntryType(void) const { return FillType; };
		virtual QWidget* getRootWidget(void) const override;

		//! @brief Creates a new painter.
		//! Caller takes ownership.
		virtual Painter2D* createPainter(void) const override;

	private:
		ColorPickButton* m_btn;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT Painter2DEditDialogLinearGradientEntry : public Painter2DEditDialogEntry {
	public:
		Painter2DEditDialogLinearGradientEntry(const Painter2D* _painter = (Painter2D*)nullptr);
		virtual ~Painter2DEditDialogLinearGradientEntry();

		virtual DialogEntryType getEntryType(void) const { return LinearType; };
		virtual QWidget* getRootWidget(void) const override { return m_cLayW; };

		//! @brief Creates a new painter.
		//! Caller takes ownership.
		virtual Painter2D* createPainter(void) const override;

	private:
		QWidget* m_cLayW;
		QVBoxLayout* m_cLay;
		intern::Painter2DEditDialogGradientBase* m_gradientBase;

		DoubleSpinBox* m_startX;
		DoubleSpinBox* m_startY;
		DoubleSpinBox* m_finalX;
		DoubleSpinBox* m_finalY;
	};


	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT Painter2DEditDialogRadialGradientEntry : public Painter2DEditDialogEntry {
		Q_OBJECT
	public:
		Painter2DEditDialogRadialGradientEntry(const Painter2D* _painter = (Painter2D*)nullptr);
		virtual ~Painter2DEditDialogRadialGradientEntry();

		virtual DialogEntryType getEntryType(void) const { return RadialType; };
		virtual QWidget* getRootWidget(void) const override { return m_cLayW; };

		//! @brief Creates a new painter.
		//! Caller takes ownership.
		virtual Painter2D* createPainter(void) const override;

	private Q_SLOTS:
		void slotFocalEnabledChanged(void);

	private:
		QWidget* m_cLayW;
		QVBoxLayout* m_cLay;
		intern::Painter2DEditDialogGradientBase* m_gradientBase;

		DoubleSpinBox* m_centerX;
		DoubleSpinBox* m_centerY;
		DoubleSpinBox* m_centerRadius;
		CheckBox* m_useFocal;
		DoubleSpinBox* m_focalX;
		DoubleSpinBox* m_focalY;
		DoubleSpinBox* m_focalRadius;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT Painter2DEditDialogReferenceEntry : public Painter2DEditDialogEntry {
	public:
		Painter2DEditDialogReferenceEntry(const Painter2D* _painter = (Painter2D*)nullptr);
		virtual ~Painter2DEditDialogReferenceEntry();

		virtual DialogEntryType getEntryType(void) const { return ReferenceType; };
		virtual QWidget* getRootWidget(void) const override;

		//! @brief Creates a new painter.
		//! Caller takes ownership.
		virtual Painter2D* createPainter(void) const override;

	private:
		ComboBox* m_comboBox;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

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

		const Painter2D* currentPainter(void) const { return m_painter; };
		Painter2D* createPainter(void) const;

		void setHasChanged(bool _changed) { m_changed = _changed; };
		bool hasChanged(void) const { return m_changed; };

	Q_SIGNALS:
		void painterChanged(void);

	private Q_SLOTS:
		void slotTypeChanged(void);
		void slotUpdate(void);
		void slotConfirm(void);

	private:
		void ini(void);
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
