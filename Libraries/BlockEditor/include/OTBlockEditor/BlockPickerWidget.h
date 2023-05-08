//! @file GraphicItemPickerWidget.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditor/BlockEditorAPIExport.h"

// Qt header
#include <QtCore/qobject.h>
#include <QtWidgets/qdockwidget.h>

class BlockConfig;

class QGraphicsItem;
class QSplitter;
class QTreeWidget;
class QTreeWidgetItem;

namespace ot {

	class GraphicsView;
	class GraphicsScene;

	class BlockPickerWidget : public QObject {
		Q_OBJECT
	public:
		BlockPickerWidget(Qt::Orientation _orientation = Qt::Vertical);
		virtual ~BlockPickerWidget();

		QWidget* widget(void);

		void setOrientation(Qt::Orientation _orientation);

	private:
		QSplitter*      m_splitter;
		QTreeWidget*    m_navigation;
		GraphicsView*   m_view;
		GraphicsScene*	m_scene;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class BlockPicker : public QDockWidget {
		Q_OBJECT
	public:
		BlockPicker(QWidget* _parentWidget = (QWidget*)nullptr);
		BlockPicker(const QString& _title, QWidget* _parentWidget = (QWidget*)nullptr);
		virtual ~BlockPicker();

		virtual void resizeEvent(QResizeEvent* _event) override;

	private:
		inline Qt::Orientation calcWidgetOrientation(void) const { return (this->width() > this->height() ? Qt::Horizontal : Qt::Vertical); };

		BlockPickerWidget* m_widget;
	};
}