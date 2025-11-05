// @otlicense
// File: ExpanderWidget.h
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
		explicit ExpanderWidget(QWidget* _parent);

		//! @brief Constructor.
		//! Creates an expander widget which is initially collapsed.
		//! @param _parent The parent widget.
		explicit ExpanderWidget(const QString& _title, QWidget* _parent);
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