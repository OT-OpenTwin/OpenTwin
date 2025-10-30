// @otlicense

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
	public:
		DoubleSpinBox(QWidget* _parentWidget = (QWidget*)nullptr);
		DoubleSpinBox(double _min, double _max, double _value, int _precision = 2, QWidget* _parentWidget = (QWidget*)nullptr);
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
		void ini(void);

		bool m_requireSignal;
		QTimer* m_timer;
	};

}