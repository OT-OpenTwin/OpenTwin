// @otlicense
// File: Painter2DEditDialogGradientBase.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once

// OpenTwin header
#include "OTGui/GuiTypes.h"
#include "OTGui/GradientPainterStop2D.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qobject.h>

class QWidget;
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
		OT_DECL_NOCOPY(Painter2DEditDialogGradientBase)
		OT_DECL_NOMOVE(Painter2DEditDialogGradientBase)
		OT_DECL_NODEFAULT(Painter2DEditDialogGradientBase)
	public:
		Painter2DEditDialogGradientBase(QVBoxLayout* _layout, const GradientPainter2D* _painter, QWidget* _parent);
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

		QWidget* m_parentWidget;
		QVBoxLayout* m_layout;
		ComboBox* m_spreadBox;

		SpinBox* m_stopsBox;
		std::list<StopEntry> m_stops;
	};

}
