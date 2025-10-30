// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetBase.h"

// Qt header
#include <QtWidgets/qabstractbutton.h>
#include <QtWidgets/qstyle.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT IndicatorWidget : public QAbstractButton, public WidgetBase {
		Q_OBJECT
		OT_DECL_NOCOPY(IndicatorWidget)
		Q_PROPERTY(Symbol checkedSymbol READ getCheckedSymbol WRITE setCheckedSymbol)
		Q_PROPERTY(Symbol uncheckedSymbol READ getUncheckedSymbol WRITE setUncheckedSymbol)
		Q_PROPERTY(bool drawFrame READ getDrawFrame WRITE setDrawFrame)
	public:
		enum Symbol {
			None,
			Checkmark,
			Cross,
			Circle
		};
		Q_ENUM(Symbol)

		IndicatorWidget(QWidget* _parent = (QWidget*)nullptr);
		IndicatorWidget(bool _checked, QWidget* _parent = (QWidget*)nullptr);
		virtual ~IndicatorWidget() = default;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Public: Vitual methods

		virtual QWidget* getQWidget(void) override { return this; };
		virtual const QWidget* getQWidget(void) const override { return this; };

		QSize sizeHint() const override;
		QSize minimumSizeHint() const override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setCheckedSymbol(Symbol _symbol);
		Symbol getCheckedSymbol() const { return m_checkedSymbol; };

		void setUncheckedSymbol(Symbol _symbol);
		Symbol getUncheckedSymbol() const { return m_uncheckedSymbol; };

		void setDrawFrame(bool enable);
		bool getDrawFrame() const { return m_drawFrame; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Protected: Overridden methods

	protected:
		void paintEvent(QPaintEvent* event) override;

		void enterEvent(QEnterEvent* event) override {
			QAbstractButton::enterEvent(event);
			update();
		}

		void leaveEvent(QEvent* event) override {
			QAbstractButton::leaveEvent(event);
			update();
		}

		void mousePressEvent(QMouseEvent* event) override {
			QAbstractButton::mousePressEvent(event);
			update();
		}

		void mouseReleaseEvent(QMouseEvent* event) override {
			QAbstractButton::mouseReleaseEvent(event);
			update();
		}

	private:
		QStyle::State getButtonState() const;

		Symbol m_checkedSymbol;
		Symbol m_uncheckedSymbol;
		bool m_drawFrame;
	};
}

