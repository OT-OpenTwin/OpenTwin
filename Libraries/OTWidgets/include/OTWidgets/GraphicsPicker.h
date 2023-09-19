//! @file GraphicsPicker.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtWidgets/qdockwidget.h>
#include <QtCore/qobject.h>
#include <QtCore/qsize.h>

// std header
#include <list>

class QTreeWidgetItem;
class QSplitter;
class QGridLayout;

namespace ot {

	class GraphicsNewEditorPackage;
	class GraphicsCollectionCfg;
	class GraphicsItemCfg;
	class GraphicsView;
	class TreeWidgetFilter;

	class OT_WIDGETS_API_EXPORT GraphicsPicker : public QObject {
		Q_OBJECT
	public:
		GraphicsPicker(Qt::Orientation _orientation = Qt::Vertical);
		virtual ~GraphicsPicker();

		QWidget* widget(void);

		void setOrientation(Qt::Orientation _orientation);
		Qt::Orientation orientation(void) const;

		void add(const ot::GraphicsNewEditorPackage& _pckg);
		void add(ot::GraphicsCollectionCfg* _topLevelCollection);
		void add(const std::list<ot::GraphicsCollectionCfg*>& _topLevelCollections);

		void clear(void);

		void setPreviewBoxSize(const QSize& _size) { m_previewSize = _size; };
		const QSize& previewBoxSize(void) const { return m_previewSize; };

	private slots:
		void slotSelectionChanged(void);

	private:
		void addCollection(ot::GraphicsCollectionCfg* _category, QTreeWidgetItem* _parentNavigationItem);
		void addCollections(const std::list<ot::GraphicsCollectionCfg*>& _categories, QTreeWidgetItem* _parentNavigationItem);

		void addItem(ot::GraphicsItemCfg* _item, QTreeWidgetItem* _parentNavigationItem);
		void addItems(const std::list<ot::GraphicsItemCfg*>& _items, QTreeWidgetItem* _parentNavigationItem);

		void storePreviewData(QTreeWidgetItem* _item, GraphicsItemCfg* _config);

		bool                  m_repaintPreviewRequired;

		QSize                 m_previewSize;
		QSplitter* m_splitter;
		TreeWidgetFilter* m_navigation;

		std::list<GraphicsView *> m_views;
		QWidget* m_viewLayoutW;
		QGridLayout* m_viewLayout;

		std::map<QTreeWidgetItem*, std::list<GraphicsItemCfg*> *> m_previewData;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT GraphicsPickerDockWidget : public QDockWidget {
		Q_OBJECT
	public:

		//! @brief Constructor
		//! @param _parentWidget The parent widget
		GraphicsPickerDockWidget(QWidget* _parentWidget = (QWidget*)nullptr);

		//! @brief Constructor
		//! @param _title The initial dock title
		//! @param _parentWidget The parent widget
		GraphicsPickerDockWidget(const QString& _title, QWidget* _parentWidget = (QWidget*)nullptr);

		//! @brief Constructor
		//! @param _customPickerWidget Provide own block picker widget instance (this dock widget takes ownership)
		//! @param _title The initial dock title
		//! @param _parentWidget The parent widget
		GraphicsPickerDockWidget(GraphicsPicker* _customPickerWidget, const QString& _title, QWidget* _parentWidget = (QWidget*)nullptr);

		virtual ~GraphicsPickerDockWidget();

		virtual void resizeEvent(QResizeEvent* _event) override;

		void clear(void);

		ot::GraphicsPicker* pickerWidget(void) { return m_widget; };


	private:
		inline Qt::Orientation calcWidgetOrientation(void) const { return (this->width() > this->height() ? Qt::Horizontal : Qt::Vertical); };

		GraphicsPicker* m_widget;
	};

}