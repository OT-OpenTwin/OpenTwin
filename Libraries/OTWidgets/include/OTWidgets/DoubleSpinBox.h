// @otlicense
// File: DoubleSpinBox.h
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
#include "OTWidgets/WidgetBase.h"

// Qt header
#include <QtWidgets/qspinbox.h>

class QTimer;

namespace ot {

	class OT_WIDGETS_API_EXPORT DoubleSpinBox : public QDoubleSpinBox, public WidgetBase {
		Q_OBJECT
		OT_DECL_NOCOPY(DoubleSpinBox)
		OT_DECL_NOMOVE(DoubleSpinBox)
		OT_DECL_NODEFAULT(DoubleSpinBox)
	public:
		explicit DoubleSpinBox(QWidget* _parentWidget);
		explicit DoubleSpinBox(double _min, double _max, double _value, int _precision, QWidget* _parentWidget);
		virtual ~DoubleSpinBox();

		virtual QWidget* getQWidget(void) override { return this; };
		virtual const QWidget* getQWidget(void) const override { return this; };

		void setChangeDelay(int _delayInMs);

	Q_SIGNALS:
		void valueChangeCompleted(double _value);

	protected:
		virtual void keyPressEvent(QKeyEvent* _event) override;
		virtual void focusOutEvent(QFocusEvent* _event) override;

	private Q_SLOTS:
		void slotValueChanged(void);
		void slotValueChangedTimeout(void);

	private:
		bool m_requireSignal;
		QTimer* m_timer;
	};

}