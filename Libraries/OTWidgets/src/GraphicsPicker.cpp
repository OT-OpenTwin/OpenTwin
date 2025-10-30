// @otlicense
// File: GraphicsPicker.cpp
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

// OpenTwin Core header
#include "OTCore/LogDispatcher.h"

// OpenTwin Gui header
#include "OTGui/GraphicsItemCfg.h"
#include "OTGui/GraphicsPackage.h"
#include "OTGui/GraphicsPickerCollectionCfg.h"

// OpenTwin Widgets header
#include "OTWidgets/Splitter.h"
#include "OTWidgets/TreeWidget.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/GraphicsItem.h"
#include "OTWidgets/ImagePainter.h"
#include "OTWidgets/GraphicsView.h"
#include "OTWidgets/GraphicsScene.h"
#include "OTWidgets/TreeWidgetItem.h"
#include "OTWidgets/GraphicsPicker.h"
#include "OTWidgets/TreeWidgetFilter.h"
#include "OTWidgets/ImagePainterManager.h"
#include "OTWidgets/GraphicsItemPreview.h"

// Qt header
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qsplitter.h>

// std header
#include <string>

namespace intern {
	enum navigationTreeColumns {
		ntTitle,
		ntCount
	};
}

ot::GraphicsPicker::GraphicsPicker(Qt::Orientation _orientation) :
	m_navigation(nullptr), m_splitter(nullptr), m_repaintPreviewRequired(false), m_previewSize(48, 48)
{
	// Create controls
	m_splitter = new Splitter(_orientation);

	m_navigation = new ot::TreeWidgetFilter;
	m_navigation->getTreeWidget()->setHeaderHidden(true);
	m_navigation->setOTWidgetFlags(ot::ApplyFilterOnTextChange);

	m_viewLayoutW = new QWidget;
	m_viewLayout = new QGridLayout(m_viewLayoutW);
	m_viewLayoutW->installEventFilter(this);

	m_splitter->addWidget(m_navigation->getQWidget());
	m_splitter->addWidget(m_viewLayoutW);

	connect(m_navigation->getTreeWidget(), &QTreeWidget::itemSelectionChanged, this, &GraphicsPicker::slotSelectionChanged);
}

ot::GraphicsPicker::~GraphicsPicker() {
	delete m_splitter;
	m_splitter = nullptr;
}

QWidget* ot::GraphicsPicker::getQWidget() {
	return m_splitter;
}

const QWidget* ot::GraphicsPicker::getQWidget() const {
	return m_splitter;
}

bool ot::GraphicsPicker::eventFilter(QObject* _watched, QEvent* _event) {
	if (_event->type() == QEvent::Resize) {
		this->rebuildPreview();
	}

	return false;
}

// ##############################################################################################################################

void ot::GraphicsPicker::setOrientation(Qt::Orientation _orientation) {
	m_splitter->setOrientation(_orientation);
}

Qt::Orientation ot::GraphicsPicker::orientation() const {
	return m_splitter->orientation();
}

void ot::GraphicsPicker::add(const ot::GraphicsPickerCollectionPackage& _pckg) {
	this->addCollections(_pckg.getCollections(), nullptr);
}

void ot::GraphicsPicker::add(const ot::GraphicsPickerCollectionCfg& _topLevelCollection) {
	this->addCollection(_topLevelCollection, nullptr);
}

void ot::GraphicsPicker::add(const std::list<ot::GraphicsPickerCollectionCfg>& _topLevelCollections) {
	this->addCollections(_topLevelCollections, nullptr);
}

void ot::GraphicsPicker::clear() {
	for (auto v : m_previews) {
		m_viewLayout->removeWidget(v.layoutWidget);
		delete v.label;
		delete v.view;
		delete v.layout;
	}
	m_previews.clear();

	for (auto d : m_previewData) {
		delete d.second;
	}
	m_previewData.clear();
	m_navigation->getTreeWidget()->clear();
}

ot::GraphicsPicker::PickerState ot::GraphicsPicker::getCurrentState() const {
	PickerState state;
	for (int i = 0; i < m_navigation->getTreeWidget()->topLevelItemCount(); i++) {
		TreeWidgetItem* itm = dynamic_cast<TreeWidgetItem*>(m_navigation->getTreeWidget()->topLevelItem(i));
		OTAssertNullptr(itm);
		this->getCurrentState(state, itm);
	}
	return state;
}

void ot::GraphicsPicker::applyState(const PickerState& _state) {
	QSignalBlocker block(m_navigation->getTreeWidget());
	for (int i = 0; i < m_navigation->getTreeWidget()->topLevelItemCount(); i++) {
		TreeWidgetItem* itm = dynamic_cast<TreeWidgetItem*>(m_navigation->getTreeWidget()->topLevelItem(i));
		OTAssertNullptr(itm);
		this->applyState(_state, itm);
	}

	this->slotSelectionChanged();
}

// ##############################################################################################################################

// Private: Slots

void ot::GraphicsPicker::slotSelectionChanged() {
	for (auto v : m_previews) {
		m_viewLayout->removeWidget(v.layoutWidget);
		delete v.label;
		delete v.view;
		delete v.layout;
	}
	m_previews.clear();

	std::list<GraphicsView *> previews;

	ImagePainterManager& pManager = ImagePainterManager::instance();

	for (QTreeWidgetItem* selectedItem : m_navigation->getTreeWidget()->selectedItems()) {
		TreeWidgetItem* itm = dynamic_cast<TreeWidgetItem*>(selectedItem);
		OTAssertNullptr(itm);
		auto it = m_previewData.find(itm);
		if (it != m_previewData.end()) {
			for (const GraphicsPickerItemInfo& info : *it->second) {
				// Preload image
				pManager.importFromFile(info.getPreviewIcon());

				PreviewBox box;
				box.view = new GraphicsItemPreview;
				box.view->setMaximumSize(m_previewSize);
				box.view->setMinimumSize(m_previewSize);
				box.view->setPainter(pManager.getPainter(info.getPreviewIcon())->createCopy());
				box.view->setItemName(info.getName());
				//box.view->setAlignment(Qt::AlignCenter);
				box.view->setOwner(m_owner);

				box.label = new QLabel(QString::fromStdString(info.getTitle()));
				box.label->setAlignment(Qt::AlignCenter);

				box.layoutWidget = new QWidget;

				box.layout = new QVBoxLayout(box.layoutWidget);
				box.layout->addWidget(box.view, 0, Qt::AlignCenter);
				box.layout->addWidget(box.label, 1, Qt::AlignTop | Qt::AlignHCenter);

				m_previews.push_back(box);
			}
		}
	}

	this->rebuildPreview();
}

// ##############################################################################################################################

// Private: Helper

void ot::GraphicsPicker::addCollection(const ot::GraphicsPickerCollectionCfg& _category, TreeWidgetItem* _parentNavigationItem) {
	TreeWidgetItem* categoryItem = nullptr;
	if (_parentNavigationItem) {
		// Check if parent has item
		for (int i = 0; i < _parentNavigationItem->childCount(); i++) {
			if (_parentNavigationItem->child(i)->text(intern::ntTitle).toLower() == QString::fromStdString(_category.getTitle()).toLower()) {
				categoryItem = dynamic_cast<TreeWidgetItem*>(_parentNavigationItem->child(i));
				OTAssertNullptr(categoryItem);
				break;
			}
		}
	}
	else {
		// Check if parent has item
		for (int i = 0; i < m_navigation->getTreeWidget()->topLevelItemCount(); i++) {
			if (m_navigation->getTreeWidget()->topLevelItem(i)->text(intern::ntTitle).toLower() == QString::fromStdString(_category.getTitle()).toLower()) {
				categoryItem = dynamic_cast<TreeWidgetItem*>(m_navigation->getTreeWidget()->topLevelItem(i));
				OTAssertNullptr(categoryItem);
				break;
			}
		}
	}

	// Item does not exist, create
	if (categoryItem == nullptr) {
		categoryItem = new TreeWidgetItem;
		categoryItem->setText(intern::ntTitle, QString::fromStdString(_category.getTitle()));

		if (_parentNavigationItem) {
			// Add item as child
			_parentNavigationItem->addChild(categoryItem);
		}
		else {
			// Add item as top level
			m_navigation->getTreeWidget()->addTopLevelItem(categoryItem);
		}
	}

	this->addCollections(_category.getChildCollections(), categoryItem);
	this->addItems(_category.getItems(), categoryItem);
}

void ot::GraphicsPicker::addCollections(const std::list<ot::GraphicsPickerCollectionCfg>& _categories, TreeWidgetItem* _parentNavigationItem) {
	for (const GraphicsPickerCollectionCfg& c : _categories) {
		this->addCollection(c, _parentNavigationItem);
	}
}

void ot::GraphicsPicker::addItem(const GraphicsPickerItemInfo& _info, TreeWidgetItem* _parentNavigationItem) {
	OTAssert(_parentNavigationItem, "nullptr provided");
	TreeWidgetItem* treeItem = nullptr;
	for (int i = 0; i < _parentNavigationItem->childCount(); i++) {
		if (_parentNavigationItem->child(i)->text(intern::ntTitle).toLower() == QString::fromStdString(_info.getTitle()).toLower()) {
			OT_LOG_W("A graphics item with the name \"" + _info.getTitle() + "\" is already a child of the collection \"" + _parentNavigationItem->text(intern::ntTitle).toStdString() + "\"");
			treeItem = dynamic_cast<TreeWidgetItem*>(_parentNavigationItem->child(i));
			OTAssertNullptr(treeItem);
			break;
		}
	}

	if (treeItem == nullptr) {
		treeItem = new TreeWidgetItem;
		treeItem->setText(intern::ntTitle, QString::fromStdString(_info.getTitle()));

		_parentNavigationItem->addChild(treeItem);

		treeItem->setHidden(true);

		this->storePreviewData(_parentNavigationItem, _info);
	}
}

void ot::GraphicsPicker::addItems(const std::list<GraphicsPickerItemInfo>& _items, TreeWidgetItem* _parentNavigationItem) {
	for (auto b : _items) {
		this->addItem(b, _parentNavigationItem);
	}
}

void ot::GraphicsPicker::storePreviewData(TreeWidgetItem* _item, const GraphicsPickerItemInfo& _info) {
	auto it = m_previewData.find(_item);
	if (it != m_previewData.end()) {
		it->second->push_back(_info);
	}
	else {
		auto lst = new std::list<GraphicsPickerItemInfo>;
		lst->push_back(_info);
		m_previewData.insert_or_assign(_item, lst);
	}
}

void ot::GraphicsPicker::rebuildPreview() {
	// Remove current previews
	for (PreviewBox& v : m_previews) {
		v.layoutWidget->setHidden(true);
		m_viewLayout->removeWidget(v.layoutWidget);
	}

	// Determine max preview width
	int maxWidth = m_previewSize.width() + 5;
	for (const PreviewBox& v : m_previews) {
		QFontMetrics fm(v.label->font());
		maxWidth = std::max(maxWidth, fm.horizontalAdvance(v.label->text()) + 5);
	}

	// Rebuild previews
	int row = 0;
	int col = 0;
	int cols = m_splitter->visibleRegion().boundingRect().width() / maxWidth;

	for (PreviewBox& v : m_previews) {
		m_viewLayout->addWidget(v.layoutWidget, row, col);
		v.layoutWidget->setHidden(false);

		col++;
		if (col >= cols) {
			col = 0;
			row++;
		}
	}
}

void ot::GraphicsPicker::getCurrentState(PickerState& _state, TreeWidgetItem* _item) const {
	if (_item->isSelected()) {
		_state.selectedItems.push_back(_item->getTreeWidgetItemPath());
	}
	if (_item->isExpanded()) {
		_state.expandedItems.push_back(_item->getTreeWidgetItemPath());
	}

	for (int i = 0; i < _item->childCount(); i++) {
		TreeWidgetItem* itm = dynamic_cast<TreeWidgetItem*>(_item->child(i));
		OTAssertNullptr(itm);
		this->getCurrentState(_state, itm);
	}
}

void ot::GraphicsPicker::applyState(const PickerState& _state, TreeWidgetItem* _item) {
	const QString path = _item->getTreeWidgetItemPath();
	if (std::find(_state.selectedItems.begin(), _state.selectedItems.end(), path) != _state.selectedItems.end()) {
		_item->setSelected(true);
	}
	else {
		_item->setSelected(false);
	}
	if (std::find(_state.expandedItems.begin(), _state.expandedItems.end(), path) != _state.expandedItems.end()) {
		_item->setExpanded(true);
	}
	else {
		_item->setExpanded(false);
	}
	for (int i = 0; i < _item->childCount(); i++) {
		TreeWidgetItem* itm = dynamic_cast<TreeWidgetItem*>(_item->child(i));
		OTAssertNullptr(itm);
		this->applyState(_state, itm);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsPickerDockWidget::GraphicsPickerDockWidget(QWidget* _parentWidget)
	: QDockWidget(_parentWidget)
{
	m_widget = new GraphicsPicker(this->calcWidgetOrientation());
	setWidget(m_widget->getQWidget());
}

ot::GraphicsPickerDockWidget::GraphicsPickerDockWidget(const QString& _title, QWidget* _parentWidget)
	: QDockWidget(_title, _parentWidget)
{
	m_widget = new GraphicsPicker(this->calcWidgetOrientation());
	setWidget(m_widget->getQWidget());
}

ot::GraphicsPickerDockWidget::GraphicsPickerDockWidget(GraphicsPicker* _customPickerWidget, const QString& _title, QWidget* _parentWidget)
	: QDockWidget(_title, _parentWidget), m_widget(_customPickerWidget)
{
	OTAssert(m_widget, "No block picker widget provided");
	setWidget(m_widget->getQWidget());
}

ot::GraphicsPickerDockWidget::~GraphicsPickerDockWidget() {
	if (m_widget) delete m_widget;
}

void ot::GraphicsPickerDockWidget::resizeEvent(QResizeEvent* _event) {
	QDockWidget::resizeEvent(_event);
	Qt::Orientation o = this->calcWidgetOrientation();
	if (o != m_widget->orientation()) m_widget->setOrientation(o);
}

void ot::GraphicsPickerDockWidget::clear() {
	m_widget->clear();
}

