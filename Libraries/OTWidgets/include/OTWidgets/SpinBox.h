//! @file SpinBox.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/QWidgetInterface.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtWidgets/qspinbox.h>

class QTimer;

namespace ot {

	class OT_WIDGETS_API_EXPORT SpinBox : public QSpinBox, public QWidgetInterface {
		Q_OBJECT
		OT_DECL_NOCOPY(SpinBox)
	public:
		SpinBox(QWidget* _parentWidget = (QWidget*)nullptr);
		SpinBox(int _min, int _max, int _value, QWidget* _parentWidget = (QWidget*)nullptr);
		virtual ~SpinBox();

		virtual QWidget* getQWidget(void) override { return this; };

		void setChangeDelay(int _delayInMs);

	Q_SIGNALS:
		void valueChangeCompleted(int _value);

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