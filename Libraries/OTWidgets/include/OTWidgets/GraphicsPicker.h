//! @file GraphicsPicker.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/BasicServiceInformation.h"
#include "OTGui/GraphicsPickerCollectionCfg.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtWidgets/qdockwidget.h>
#include <QtCore/qobject.h>
#include <QtCore/qsize.h>

// std header
#include <list>

class QLabel;
class QWidget;
class QGridLayout;
class QVBoxLayout;
class QTreeWidgetItem;

namespace ot {

	class Splitter;
	class GraphicsItemCfg;
	class TreeWidgetFilter;
	class GraphicsItemPreview;
	class GraphicsPickerCollectionPackage;

	class OT_WIDGETS_API_EXPORT GraphicsPicker : public QObject {
		Q_OBJECT
	public:
		GraphicsPicker(Qt::Orientation _orientation = Qt::Vertical);
		virtual ~GraphicsPicker();

		QWidget* pickerWidget(void);

		void setOrientation(Qt::Orientation _orientation);
		Qt::Orientation orientation(void) const;

		void add(const ot::GraphicsPickerCollectionPackage& _pckg);
		void add(ot::GraphicsPickerCollectionCfg* _topLevelCollection);
		void add(const std::list<ot::GraphicsPickerCollectionCfg*>& _topLevelCollections);

		void clear(void);

		void setPreviewBoxSize(const QSize& _size) { m_previewSize = _size; };
		const QSize& previewBoxSize(void) const { return m_previewSize; };

		void setOwner(const BasicServiceInformation& _owner) { m_owner = _owner; };

		//! \brief Returns the current owner of the graphics picker.
		//! The owner information will be added to the mime data when dragging an item.
		const BasicServiceInformation& getOwner(void) const { return m_owner; };

	private Q_SLOTS:
		void slotSelectionChanged(void);

	private:
		void addCollection(ot::GraphicsPickerCollectionCfg* _category, QTreeWidgetItem* _parentNavigationItem);
		void addCollections(const std::list<ot::GraphicsPickerCollectionCfg*>& _categories, QTreeWidgetItem* _parentNavigationItem);

		void addItem(const GraphicsPickerItemInformation& _info, QTreeWidgetItem* _parentNavigationItem);
		void addItems(const std::list<GraphicsPickerItemInformation>& _info, QTreeWidgetItem* _parentNavigationItem);

		void storePreviewData(QTreeWidgetItem* _item, const GraphicsPickerItemInformation& _info);

		struct PreviewBox {
			QWidget* layoutWidget;
			QVBoxLayout* layout;
			GraphicsItemPreview* view;
			QLabel* label;
		};

		bool                  m_repaintPreviewRequired;

		BasicServiceInformation m_owner;

		QSize                 m_previewSize;
		Splitter* m_splitter;
		TreeWidgetFilter* m_navigation;

		std::list<PreviewBox> m_previews;
		QWidget* m_viewLayoutW;
		QGridLayout* m_viewLayout;

		std::map<QTreeWidgetItem*, std::list<GraphicsPickerItemInformation>*> m_previewData;
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
