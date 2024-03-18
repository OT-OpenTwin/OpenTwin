//! @file GraphicsPicker.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OTGui/GraphicsPackage.h"
#include "OTGui/GraphicsPickerCollectionCfg.h"
#include "OTWidgets/TreeWidget.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/GraphicsItem.h"
#include "OTWidgets/GraphicsView.h"
#include "OTWidgets/GraphicsScene.h"
#include "OTWidgets/GraphicsPicker.h"
#include "OTWidgets/GraphicsFactory.h"
#include "OTWidgets/TreeWidgetFilter.h"
#include "OTWidgets/GraphicsItemPreview.h"

// Qt header
#include <QtWidgets/qsplitter.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qlabel.h>

// std header
#include <string>

namespace intern {
	enum navigationTreeColumns {
		ntTitle,
		ntCount
	};
}

ot::GraphicsPicker::GraphicsPicker(Qt::Orientation _orientation)
	: m_navigation(nullptr), m_splitter(nullptr), m_repaintPreviewRequired(false), m_previewSize(48, 48)
{
	// Create controls
	m_splitter = new QSplitter(_orientation);

	m_navigation = new ot::TreeWidgetFilter;
	m_navigation->treeWidget()->setHeaderHidden(true);
	m_navigation->setOTWidgetFlags(ot::ApplyFilterOnTextChange);

	m_viewLayoutW = new QWidget;
	m_viewLayout = new QGridLayout(m_viewLayoutW);

	m_splitter->addWidget(m_navigation->getQWidget());
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

void ot::GraphicsPicker::add(const ot::GraphicsPickerCollectionPackage& _pckg) {
	this->addCollections(_pckg.collections(), nullptr);
}

void ot::GraphicsPicker::add(ot::GraphicsPickerCollectionCfg* _topLevelCollection) {
	this->addCollection(_topLevelCollection, nullptr);
}

void ot::GraphicsPicker::add(const std::list<ot::GraphicsPickerCollectionCfg*>& _topLevelCollections) {
	this->addCollections(_topLevelCollections, nullptr);
}

void ot::GraphicsPicker::clear(void) {
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
	m_navigation->treeWidget()->clear();
}

// ##############################################################################################################################

// Private: Slots

void ot::GraphicsPicker::slotSelectionChanged(void) {
	for (auto v : m_previews) {
		m_viewLayout->removeWidget(v.layoutWidget);
		delete v.label;
		delete v.view;
		delete v.layout;
	}
	m_previews.clear();

	std::list<GraphicsView *> previews;

	for (auto itm : m_navigation->treeWidget()->selectedItems()) {
		auto it = m_previewData.find(itm);
		if (it != m_previewData.end()) {
			for (const GraphicsPickerCollectionCfg::ItemInformation& info : *it->second) {
				PreviewBox box;

				box.view = new GraphicsItemPreview;
				box.view->setMaximumSize(m_previewSize);
				box.view->setMinimumSize(m_previewSize);
				box.view->setPixmap(IconManager::instance().getIcon(QString::fromStdString(info.previewIcon)).pixmap(m_previewSize));
				box.view->setItemName(info.itemName);
				box.view->setAlignment(Qt::AlignCenter);

				box.label = new QLabel(QString::fromStdString(info.itemTitle));
				box.label->setAlignment(Qt::AlignCenter);

				box.layoutWidget = new QWidget;

				box.layout = new QVBoxLayout(box.layoutWidget);
				box.layout->addWidget(box.view, 0, Qt::AlignCenter);
				box.layout->addWidget(box.label, 1, Qt::AlignTop | Qt::AlignHCenter);

				m_viewLayout->addWidget(box.layoutWidget);

				m_previews.push_back(box);
			}
		}
	}
}

// ##############################################################################################################################

// Private: Helper

void ot::GraphicsPicker::addCollection(ot::GraphicsPickerCollectionCfg* _category, QTreeWidgetItem* _parentNavigationItem) {
	OTAssert(_category, "nullptr provided");

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

void ot::GraphicsPicker::addCollections(const std::list<ot::GraphicsPickerCollectionCfg*>& _categories, QTreeWidgetItem* _parentNavigationItem) {
	for (auto c : _categories)
	{
		this->addCollection(c, _parentNavigationItem);
	}
}

void ot::GraphicsPicker::addItem(const GraphicsPickerCollectionCfg::ItemInformation& _info, QTreeWidgetItem* _parentNavigationItem) {
	OTAssert(_parentNavigationItem, "nullptr provided");

	QTreeWidgetItem* treeItem = nullptr;
	for (int i = 0; i < _parentNavigationItem->childCount(); i++) {
		if (_parentNavigationItem->child(i)->text(intern::ntTitle).toLower() == QString::fromStdString(_info.itemTitle).toLower()) {
			OT_LOG_W("A graphics item with the name \"" + _info.itemTitle + "\" is already a child of the collection \"" + _parentNavigationItem->text(intern::ntTitle).toStdString() + "\"");
			treeItem = _parentNavigationItem->child(i);
			break;
		}
	}

	if (treeItem == nullptr) {
		treeItem = new QTreeWidgetItem;
		treeItem->setText(intern::ntTitle, QString::fromStdString(_info.itemTitle));

		_parentNavigationItem->addChild(treeItem);

		treeItem->setHidden(true);

		this->storePreviewData(_parentNavigationItem, _info);
	}
}

void ot::GraphicsPicker::addItems(const std::list<GraphicsPickerCollectionCfg::ItemInformation>& _items, QTreeWidgetItem* _parentNavigationItem) {
	for (auto b : _items) {
		this->addItem(b, _parentNavigationItem);
	}
}

void ot::GraphicsPicker::storePreviewData(QTreeWidgetItem* _item, const GraphicsPickerCollectionCfg::ItemInformation& _info) {
	auto it = m_previewData.find(_item);
	if (it != m_previewData.end()) {
		it->second->push_back(_info);
	}
	else {
		auto lst = new std::list<GraphicsPickerCollectionCfg::ItemInformation>;
		lst->push_back(_info);
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
	OTAssert(m_widget, "No block picker widget provided");
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

