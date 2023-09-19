//! @file GraphicsPicker.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/GraphicsPicker.h"
#include "OTWidgets/TreeWidget.h"
#include "OTWidgets/TreeWidgetFilter.h"
#include "OTWidgets/GraphicsItem.h"
#include "OTWidgets/GraphicsView.h"
#include "OTWidgets/GraphicsScene.h"
#include "OTWidgets/GraphicsFactory.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OTGui/GraphicsCollectionCfg.h"
#include "OTGui/GraphicsPackage.h"
#include "OpenTwinCore/Logger.h"

// Qt header
#include <QtWidgets/qsplitter.h>
#include <QtWidgets/qlayout.h>

// std header
#include <string>

namespace intern {
	enum navigationTreeColumns {
		ntTitle,
		ntCount
	};
}

ot::GraphicsPicker::GraphicsPicker(Qt::Orientation _orientation) : m_navigation(nullptr), m_splitter(nullptr), m_repaintPreviewRequired(false), m_previewSize(50, 50) {
	// Create controls
	m_splitter = new QSplitter(_orientation);

	m_navigation = new ot::TreeWidgetFilter;
	m_navigation->treeWidget()->setHeaderHidden(true);
	m_navigation->setWidgetFlags(ot::ApplyFilterOnTextChange);

	m_viewLayoutW = new QWidget;
	m_viewLayout = new QGridLayout(m_viewLayoutW);

	m_splitter->addWidget(m_navigation->getWidget());
	m_splitter->addWidget(m_viewLayoutW);

	connect(m_navigation->treeWidget(), &QTreeWidget::itemSelectionChanged, this, &GraphicsPicker::slotSelectionChanged);
}

ot::GraphicsPicker::~GraphicsPicker() {

}

// ##############################################################################################################################

QWidget* ot::GraphicsPicker::widget(void) {
	return m_splitter;
}

// ##############################################################################################################################

void ot::GraphicsPicker::setOrientation(Qt::Orientation _orientation) {
	m_splitter->setOrientation(_orientation);
}

Qt::Orientation ot::GraphicsPicker::orientation(void) const {
	return m_splitter->orientation();
}

void ot::GraphicsPicker::add(const ot::GraphicsCollectionPackage& _pckg) {
	this->addCollections(_pckg.collections(), nullptr);
}

void ot::GraphicsPicker::add(ot::GraphicsCollectionCfg* _topLevelCollection) {
	this->addCollection(_topLevelCollection, nullptr);
}

void ot::GraphicsPicker::add(const std::list<ot::GraphicsCollectionCfg*>& _topLevelCollections) {
	this->addCollections(_topLevelCollections, nullptr);
}

void ot::GraphicsPicker::clear(void) {
	m_navigation->treeWidget()->clear();
	for (auto d : m_previewData) {
		for (auto e : *d.second) delete e;
		delete d.second;
	}
	m_previewData.clear();
	for (auto v : m_views) {
		m_viewLayout->removeWidget(v);
		delete v;
	}
	m_views.clear();
}

// ##############################################################################################################################

// Private: Slots

void ot::GraphicsPicker::slotSelectionChanged(void) {
	for (auto v : m_views) {
		m_viewLayout->removeWidget(v);
		delete v;
	}
	m_views.clear();

	std::list<GraphicsView *> previews;

	for (auto itm : m_navigation->treeWidget()->selectedItems()) {
		auto it = m_previewData.find(itm);
		if (it != m_previewData.end()) {
			for (auto bCfg : *it->second) {
				// Construct block
				ot::GraphicsItem* newItem = ot::GraphicsFactory::itemFromConfig(bCfg);
				
				if (newItem) {					
					GraphicsView* newView = new GraphicsView;
					newView->setMaximumSize(m_previewSize);
					newView->setMinimumSize(m_previewSize);
					newView->setDragMode(QGraphicsView::NoDrag);
					
					newView->getGraphicsScene()->setBackgroundBrush(QBrush());
					newView->getGraphicsScene()->addItem(newItem->getQGraphicsItem());
					newItem->setGraphicsItemFlags(ot::GraphicsItem::ItemPreviewContext);

					newView->viewAll();

					m_viewLayout->addWidget(newView);
					m_views.push_back(newView);
				}
				else {
					OT_LOG_E("Failed to create preview item from factory");
				}
			}
		}
	}
}

// ##############################################################################################################################

// Private: Helper

void ot::GraphicsPicker::addCollection(ot::GraphicsCollectionCfg* _category, QTreeWidgetItem* _parentNavigationItem) {
	otAssert(_category, "nullptr provided");

	QTreeWidgetItem* categoryItem = nullptr;
	if (_parentNavigationItem) {
		// Check if parent has item
		for (int i = 0; i < _parentNavigationItem->childCount(); i++) {
			if (_parentNavigationItem->child(i)->text(intern::ntTitle).toLower() == QString::fromStdString(_category->title()).toLower()) {
				categoryItem = _parentNavigationItem->child(i);
				break;
			}
		}
	}
	else {
		// Check if parent has item
		for (int i = 0; i < m_navigation->treeWidget()->topLevelItemCount(); i++) {
			if (m_navigation->treeWidget()->topLevelItem(i)->text(intern::ntTitle).toLower() == QString::fromStdString(_category->title()).toLower()) {
				categoryItem = m_navigation->treeWidget()->topLevelItem(i);
				break;
			}
		}
	}

	// Item does not exist, create
	if (categoryItem == nullptr) {
		categoryItem = new QTreeWidgetItem;
		categoryItem->setText(intern::ntTitle, QString::fromStdString(_category->title()));

		if (_parentNavigationItem) {
			// Add item as child
			_parentNavigationItem->addChild(categoryItem);
		}
		else {
			// Add item as top level
			m_navigation->treeWidget()->addTopLevelItem(categoryItem);
		}
	}

	this->addCollections(_category->childCollections(), categoryItem);
	this->addItems(_category->items(), categoryItem);
}

void ot::GraphicsPicker::addCollections(const std::list<ot::GraphicsCollectionCfg*>& _categories, QTreeWidgetItem* _parentNavigationItem) {
	for (auto c : _categories)
	{
		this->addCollection(c, _parentNavigationItem);
	}
}

void ot::GraphicsPicker::addItem(ot::GraphicsItemCfg* _item, QTreeWidgetItem* _parentNavigationItem) {
	otAssert(_item, "nullptr provided");
	otAssert(_parentNavigationItem, "nullptr provided");

	QTreeWidgetItem* treeItem = nullptr;
	for (int i = 0; i < _parentNavigationItem->childCount(); i++) {
		if (_parentNavigationItem->child(i)->text(intern::ntTitle).toLower() == QString::fromStdString(_item->title()).toLower()) {
			OT_LOG_W("A graphics item with the name \"" + _item->title() + "\" is already a child of the collection \"" + _parentNavigationItem->text(intern::ntTitle).toStdString() + "\"");
			treeItem = _parentNavigationItem->child(i);
			break;
		}
	}

	if (treeItem == nullptr) {
		treeItem = new QTreeWidgetItem;
		treeItem->setText(intern::ntTitle, QString::fromStdString(_item->title()));

		OT_rJSON_createDOC(doc);
		OT_rJSON_createValueObject(obj);
		_item->addToJsonObject(doc, obj);

		ot::GraphicsItemCfg* config = nullptr;
		try {
			config = ot::SimpleFactory::instance().createType<ot::GraphicsItemCfg>(obj);
			if (config) {
				config->setFromJsonObject(obj);
				_parentNavigationItem->addChild(treeItem);
				treeItem->setHidden(true);
				this->storePreviewData(_parentNavigationItem, config);
			}
			else {
				OT_LOG_EA("Failed to create item configuration");
			}
		}
		catch (const std::exception& _e) {
			OT_LOG_EAS("Error while creating graphics object from configuration" + std::string(_e.what()));
			delete treeItem;
			if (config) delete config;
			return;
		}
		catch (...) {
			OT_LOG_EA("Error while creating graphics item from configuration: Unknown error");
			delete treeItem;
			if (config) delete config;
			return;
		}
	}
}

void ot::GraphicsPicker::addItems(const std::list<ot::GraphicsItemCfg*>& _items, QTreeWidgetItem* _parentNavigationItem) {
	for (auto b : _items) {
		this->addItem(b, _parentNavigationItem);
	}
}

void ot::GraphicsPicker::storePreviewData(QTreeWidgetItem* _item, GraphicsItemCfg* _config) {
	auto it = m_previewData.find(_item);
	if (it != m_previewData.end()) {
		it->second->push_back(_config);
	}
	else {
		auto lst = new std::list<GraphicsItemCfg*>;
		lst->push_back(_config);
		m_previewData.insert_or_assign(_item, lst);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsPickerDockWidget::GraphicsPickerDockWidget(QWidget* _parentWidget)
	: QDockWidget(_parentWidget)
{
	m_widget = new GraphicsPicker(this->calcWidgetOrientation());
	setWidget(m_widget->widget());
}

ot::GraphicsPickerDockWidget::GraphicsPickerDockWidget(const QString& _title, QWidget* _parentWidget)
	: QDockWidget(_title, _parentWidget)
{
	m_widget = new GraphicsPicker(this->calcWidgetOrientation());
	setWidget(m_widget->widget());
}

ot::GraphicsPickerDockWidget::GraphicsPickerDockWidget(GraphicsPicker* _customPickerWidget, const QString& _title, QWidget* _parentWidget)
	: QDockWidget(_title, _parentWidget), m_widget(_customPickerWidget)
{
	otAssert(m_widget, "No block picker widget provided");
	setWidget(m_widget->widget());
}

ot::GraphicsPickerDockWidget::~GraphicsPickerDockWidget() {
	if (m_widget) delete m_widget;
}

void ot::GraphicsPickerDockWidget::resizeEvent(QResizeEvent* _event) {
	QDockWidget::resizeEvent(_event);
	Qt::Orientation o = this->calcWidgetOrientation();
	if (o != m_widget->orientation()) m_widget->setOrientation(o);
}

void ot::GraphicsPickerDockWidget::clear(void) {
	m_widget->clear();
}

