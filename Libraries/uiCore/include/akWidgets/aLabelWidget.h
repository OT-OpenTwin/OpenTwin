// @otlicense

#pragma once

// Qt header
#include <qlabel.h>					// Base class
#include <qstring.h>				// QString

// AK header
#include <akWidgets/aWidget.h>
#include <akCore/globalDataTypes.h>

namespace ak {

	class UICORE_API_EXPORT aLabelWidget : public QLabel, public aWidget
	{
		Q_OBJECT
	public:
		aLabelWidget(QWidget * _parent = nullptr);

		aLabelWidget(const QString & _text, QWidget * _parent = nullptr);

		virtual ~aLabelWidget();

		// #############################################################################################################################
		// Base class functions

		//! @brief Will return the widgets widget to display it
		virtual QWidget * widget(void) override;

	Q_SIGNALS:
		void clicked();

	protected:
		bool event(QEvent *myEvent);

	private:
		aLabelWidget(const aLabelWidget &) = delete;
		aLabelWidget & operator = (const aLabelWidget &) = delete;
	};
} // namespace ak
