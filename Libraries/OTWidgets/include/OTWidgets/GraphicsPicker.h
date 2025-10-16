//! @file GraphicsPicker.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/BasicServiceInformation.h"
#include "OTGui/GraphicsPickerCollectionCfg.h"
#include "OTWidgets/WidgetBase.h"
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

namespace ot {

	class Splitter;
	class TreeWidgetItem;
	class GraphicsItemCfg;
	class TreeWidgetFilter;
	class GraphicsItemPreview;
	class GraphicsPickerCollectionPackage;

	class OT_WIDGETS_API_EXPORT GraphicsPicker : public QObject, public WidgetBase {
		Q_OBJECT
		OT_DECL_NOCOPY(GraphicsPicker)
		OT_DECL_NOMOVE(GraphicsPicker)
	public:
		struct PickerState {
			QStringList expandedItems;
			QStringList selectedItems;
		};

		GraphicsPicker(Qt::Orientation _orientation = Qt::Vertical);
		virtual ~GraphicsPicker();

		virtual QWidget* getQWidget() override;
		virtual const QWidget* getQWidget() const override;

		virtual bool eventFilter(QObject* _watched, QEvent* _event) override;

		void setOrientation(Qt::Orientation _orientation);
		Qt::Orientation orientation() const;

		void add(const GraphicsPickerCollectionPackage& _pckg);
		void add(const GraphicsPickerCollectionCfg& _topLevelCollection);
		void add(const std::list<GraphicsPickerCollectionCfg>& _topLevelCollections);

		void clear();

		PickerState getCurrentState() const;
		void applyState(const PickerState& _state);

		void setPreviewBoxSize(const QSize& _size) { m_previewSize = _size; };
		const QSize& previewBoxSize() const { return m_previewSize; };

		void setOwner(const BasicServiceInformation& _owner) { m_owner = _owner; };

		//! @brief Returns the current owner of the graphics picker.
		//! The owner information will be added to the mime data when dragging an item.
		const BasicServiceInformation& getOwner() const { return m_owner; };

	private Q_SLOTS:
		void slotSelectionChanged();

	private:
		void addCollection(const GraphicsPickerCollectionCfg& _category, TreeWidgetItem* _parentNavigationItem);
		void addCollections(const std::list<GraphicsPickerCollectionCfg>& _categories, TreeWidgetItem* _parentNavigationItem);

		void addItem(const GraphicsPickerItemInfo& _info, TreeWidgetItem* _parentNavigationItem);
		void addItems(const std::list<GraphicsPickerItemInfo>& _info, TreeWidgetItem* _parentNavigationItem);

		void storePreviewData(TreeWidgetItem* _item, const GraphicsPickerItemInfo& _info);

		void rebuildPreview();

		void getCurrentState(PickerState& _state, TreeWidgetItem* _item) const;
		void applyState(const PickerState& _state, TreeWidgetItem* _item);

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

		std::map<TreeWidgetItem*, std::list<GraphicsPickerItemInfo>*> m_previewData;
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

		void clear();

		ot::GraphicsPicker* pickerWidget() { return m_widget; };

	private:
		inline Qt::Orientation calcWidgetOrientation() const { return (this->width() > this->height() ? Qt::Horizontal : Qt::Vertical); };

		GraphicsPicker* m_widget;
	};

}
