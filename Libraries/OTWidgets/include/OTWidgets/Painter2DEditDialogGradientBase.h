// @otlicense

#pragma once

// OpenTwin header
#include "OTGui/GuiTypes.h"
#include "OTGui/GradientPainterStop2D.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qobject.h>

class QGroupBox;
class QVBoxLayout;

namespace ot {

	class SpinBox;
	class ComboBox;
	class DoubleSpinBox;
	class ColorPickButton;
	class GradientPainter2D;

	class OT_WIDGETS_API_EXPORT Painter2DEditDialogGradientBase : public QObject {
		Q_OBJECT
	public:
		Painter2DEditDialogGradientBase(QVBoxLayout* _layout, const GradientPainter2D* _painter);
		virtual ~Painter2DEditDialogGradientBase();

		inline void addStop(double _pos, const ot::Color& _color) { this->addStop(GradientPainterStop2D(_pos, _color)); };
		void addStop(const GradientPainterStop2D& _stop);
		void addStops(const std::vector<GradientPainterStop2D>& _stops);
		std::vector<GradientPainterStop2D>stops() const;
		ot::GradientSpread gradientSpread();

	Q_SIGNALS:
		void valuesChanged();

	private Q_SLOTS:
		void slotCountChanged(int _newCount);
		void slotValueChanged();

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
