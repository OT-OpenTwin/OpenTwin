// @otlicense
// File: GraphicsPicker.h
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

class QWidget;
class QGridLayout;
class QVBoxLayout;

namespace ot {

	class Label;
	class Splitter;
	class FlowLayout;
	class TreeWidgetItem;
	class GraphicsItemCfg;
	class TreeWidgetFilter;
	class GraphicsItemPreview;
	class GraphicsPickerCollectionPackage;

	class OT_WIDGETS_API_EXPORT GraphicsPicker : public QObject, public WidgetBase {
		Q_OBJECT
		OT_DECL_NOCOPY(GraphicsPicker)
		OT_DECL_NOMOVE(GraphicsPicker)
		OT_DECL_NODEFAULT(GraphicsPicker)
	public:
		struct PickerState {
			QStringList expandedItems;
			QStringList selectedItems;
		};

		explicit GraphicsPicker(QWidget* _parent);
		explicit GraphicsPicker(Qt::Orientation _orientation, QWidget* _parent);
		virtual ~GraphicsPicker();

		virtual QWidget* getQWidget() override;
		virtual const QWidget* getQWidget() const override;

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

		void setKey(const std::string& _key) { m_key = _key; };
		const std::string& getKey() const { return m_key; };

	private Q_SLOTS:
		void slotSelectionChanged();

	private:
		void addCollection(const GraphicsPickerCollectionCfg& _category, TreeWidgetItem* _parentNavigationItem);
		void addCollections(const std::list<GraphicsPickerCollectionCfg>& _categories, TreeWidgetItem* _parentNavigationItem);

		void addItem(const GraphicsPickerItemInfo& _info, TreeWidgetItem* _parentNavigationItem);
		void addItems(const std::list<GraphicsPickerItemInfo>& _info, TreeWidgetItem* _parentNavigationItem);

		void storePreviewData(TreeWidgetItem* _item, const GraphicsPickerItemInfo& _info);

		void getCurrentState(PickerState& _state, TreeWidgetItem* _item) const;
		void applyState(const PickerState& _state, TreeWidgetItem* _item);

		struct PreviewBox {
			QWidget* layoutWidget = nullptr;
			QVBoxLayout* layout = nullptr;
			GraphicsItemPreview* view = nullptr;
			Label* label = nullptr;
		};

		bool                  m_repaintPreviewRequired;

		std::string          m_key;

		QSize                 m_previewSize;
		Splitter* m_splitter;
		TreeWidgetFilter* m_navigation;

		std::list<PreviewBox> m_previews;
		QWidget* m_viewLayoutW;
		FlowLayout* m_viewLayout;

		std::map<TreeWidgetItem*, std::list<GraphicsPickerItemInfo>*> m_previewData;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT GraphicsPickerDockWidget : public QDockWidget {
		Q_OBJECT
		OT_DECL_NOCOPY(GraphicsPickerDockWidget)
		OT_DECL_NOMOVE(GraphicsPickerDockWidget)
		OT_DECL_NODEFAULT(GraphicsPickerDockWidget)
	public:

		//! @brief Constructor
		//! @param _parentWidget The parent widget
		explicit GraphicsPickerDockWidget(QWidget* _parentWidget);

		//! @brief Constructor
		//! @param _title The initial dock title
		//! @param _parentWidget The parent widget
		explicit GraphicsPickerDockWidget(const QString& _title, QWidget* _parentWidget);

		//! @brief Constructor
		//! @param _customPickerWidget Provide own block picker widget instance (this dock widget takes ownership)
		//! @param _title The initial dock title
		//! @param _parentWidget The parent widget
		explicit GraphicsPickerDockWidget(GraphicsPicker* _customPickerWidget, const QString& _title, QWidget* _parentWidget);

		virtual ~GraphicsPickerDockWidget();

		virtual void resizeEvent(QResizeEvent* _event) override;

		void clear();

		ot::GraphicsPicker* pickerWidget() { return m_widget; };

	private:
		inline Qt::Orientation calcWidgetOrientation() const { return (this->width() > this->height() ? Qt::Horizontal : Qt::Vertical); };

		GraphicsPicker* m_widget;
	};

}
