// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetBase.h"

// Qt header
#include <QtWidgets/qwidget.h>

class QFrame;
class QScrollArea;
class QVBoxLayout;
class QParallelAnimationGroup;

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

		// ###########################################################################################################################################################################################################################################################################################################################

		// Public: Slots

	public Q_SLOTS:
		void expand();
		void collapse();
		void toggle();
		void animationFinished();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Protected: Events

	protected:

		void resizeEvent(QResizeEvent* _event) override;
		void mousePressEvent(QMouseEvent* _event) override;
		void mouseMoveEvent(QMouseEvent* _event) override;
		void mouseReleaseEvent(QMouseEvent* _event) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Helper

	private:
		bool isInResizeHandleArea(const QPoint& _pos) const;
		void updateChild();

		ot::ToolButton* m_toggleButton;
		QFrame* m_headerLine;
		QParallelAnimationGroup* m_toggleAnimation;
		QWidget* m_childAreaWidget;
		QVBoxLayout* m_childArea;
		QFrame* m_handle;
		QWidget* m_currentWidget;

		int m_animationDuration;

		bool m_expanded;
		int m_minimumExpandedHeight;
		int m_expandedHeight;

		bool m_resizing;
		QPoint m_dragStartPosition;
		int m_dragStartHeight;

		const int c_titleHeight;
		const int c_handleHeight;
	};
}