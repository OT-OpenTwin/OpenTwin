//! @file ExpanderWidget.h
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/WidgetBase.h"

// Qt header
#include <QtWidgets/qwidget.h>

class QFrame;
class QScrollArea;
class QVBoxLayout;
class QPropertyAnimation;

namespace ot {

	class ToolButton;

	//! @brief The ExpanderWidget is a widget that can expand and collapse to show or hide its content.
	//! It consists of a header with a toggle button and a content area that can hold any widget.
	class OT_WIDGETS_API_EXPORT ExpanderWidget : public QWidget, public WidgetBase {
		Q_OBJECT
		OT_DECL_NOCOPY(ExpanderWidget)
	public:
		//! @brief Constructor.
		//! Creates an expander widget which is initially collapsed.
		//! @param _title The title of the expander widget.
		//! @param _parent The parent widget.
		ExpanderWidget(const QString& _title = QString(), QWidget* _parent = (QWidget*)nullptr);
		virtual ~ExpanderWidget();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Virtual methods

		virtual QWidget* getQWidget() override { return this; };
		virtual const QWidget* getQWidget() const override { return this; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setWidget(QWidget* _widget);

		void setTitle(const QString& _title);
		QString getTitle() const;

		void setMinExpandedHeight(int _height);
		int getMinExpandedHeight() const { return m_minExpandedHeight; }

		// ###########################################################################################################################################################################################################################################################################################################################

		// Public: Slots

	public Q_SLOTS:
		void updateAnimationLimits();
		void expand();
		void collapse();
		void toggle();
		void animationFinished();

	private:
		ot::ToolButton* m_toggleButton;
		QFrame* m_headerLine;
		QPropertyAnimation* m_toggleAnimation;
		QWidget* m_childAreaWidget;
		QVBoxLayout* m_childArea;
		QWidget* m_currentWidget;
		int m_animationDuration;
		bool m_expanded;
		int m_lastHeightHint;
		int m_minExpandedHeight;
	};
}