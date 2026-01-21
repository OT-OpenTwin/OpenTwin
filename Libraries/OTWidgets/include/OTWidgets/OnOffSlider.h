// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetBase.h"

// Qt header
#include <QtWidgets/qwidget.h>

class QPropertyAnimation;

namespace ot {

	class OT_WIDGETS_API_EXPORT OnOffSlider : public QWidget {
		Q_OBJECT
		Q_PROPERTY(qreal knobPos READ getKnobPos WRITE setKnobPos)
	public:
		explicit OnOffSlider(QWidget* _parent = (QWidget*)nullptr);

		void setChecked(bool _checked);
		bool getChecked() const { return m_checked; };

		void setShowLabels(bool _on);
		bool getShowLabels() const { return m_showLabels; };

		QSize sizeHint() const override { return { 40, 18 }; };

	Q_SIGNALS:
		void toggled(bool _checked);

	public Q_SLOTS:
		void toggle();

	protected:
		void mousePressEvent(QMouseEvent* _event) override;
		void paintEvent(QPaintEvent* _event) override;


	private:
		void startAnimation();

		void setKnobPos(qreal _pos);
		qreal getKnobPos() const { return m_knobPos; };

	private:
		bool m_checked;
		bool m_showLabels;
		qreal m_knobPos;
		QPropertyAnimation* m_animation;
	};

}