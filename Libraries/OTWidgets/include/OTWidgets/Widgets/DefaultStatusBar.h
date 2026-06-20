// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/Widgets/StatusBar.h"

// Qt header
#include <QtCore/qtimer.h>
#include <QtWidgets/qstatusbar.h>

class QHBoxLayout;

namespace ot
{

	class Label;
	class ProgressBar;

	class OT_WIDGETS_API_EXPORT DefaultStatusBar : public StatusBar
	{
		Q_OBJECT
		OT_DECL_NOCOPY(DefaultStatusBar)
		OT_DECL_NOMOVE(DefaultStatusBar)
		OT_DECL_NODEFAULT(DefaultStatusBar)
	public:
		DefaultStatusBar(QWidget* _parent);
		virtual ~DefaultStatusBar();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Widget management

		//! @brief Adds a widget to the left side of the status bar.
		//! @param _widget The widget to add. The status bar takes ownership of the widget.
		void addLeftWidget(QWidget* _widget);

		//! @brief Removes a widget from the left side of the status bar.
		//! @param _widget The widget to remove. The caller takes ownership of the widget.
		void removeLeftWidget(QWidget* _widget);

		//! @brief Adds a widget to the right side of the status bar.
		//! @param _widget The widget to add. The status bar takes ownership of the widget.
		void addRightWidget(QWidget* _widget);

		//! @brief Removes a widget from the right side of the status bar.
		//! @param _widget The widget to remove. The caller takes ownership of the widget.
		void removeRightWidget(QWidget* _widget);

		//! @brief Adds a widget to the right corner of the status bar.
		//! @param _widget The widget to add. The status bar takes ownership of the widget.
		void addRightCornerWidget(QWidget* _widget);

		//! @brief Removes a widget from the right corner of the status bar.
		//! @param _widget The widget to remove. The caller takes ownership of the widget.
		void removeRightCornerWidget(QWidget* _widget);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Public slots

	public Q_SLOTS:
		void showStateMessage(const QString& _message);
		void hideStateMessageDelayed(int _timeoutMs = 5000);
		void hideStateMessage();

		//! @brief Shows a progress bar with the given message and progress values.
		//! If the progress value is negative, an infinite progress bar is shown.
		void showProgress(const QString& _message, int _progress);
		void hideProgressDelayed(int _timeoutMs = 5000);
		void hideProgress();

		void setProgressValue(int _progress);

	private:
		QHBoxLayout* m_stateLayout;
		QHBoxLayout* m_customLeftLayout;
		QHBoxLayout* m_progressLayout;
		QHBoxLayout* m_customRightLayout;
		QHBoxLayout* m_rightCornerLayout;

		QTimer m_stateHideTimer;
		Label* m_stateLabel;

		QTimer m_progressHideTimer;
		ProgressBar* m_progressBar;
		Label* m_progressLabel;

	};
}