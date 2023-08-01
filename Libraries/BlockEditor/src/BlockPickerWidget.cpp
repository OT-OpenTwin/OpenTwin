//! @file BlockPickerWidget.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTBlockEditor/BlockPickerWidget.h"
#include "OTBlockEditor/Block.h"
#include "OTBlockEditor/BlockFactory.h"
#include "OTBlockEditor/BlockNetwork.h"
#include "OTBlockEditor/BlockNetworkEditor.h"
#include "OTBlockEditorAPI/BlockConfiguration.h"
#include "OTBlockEditorAPI/BlockConfigurationFactory.h"
#include "OTBlockEditorAPI/BlockCategoryConfiguration.h"
#include "OTWidgets/TreeWidgetFilter.h"
#include "OTWidgets/TreeWidget.h"
#include "OpenTwinCore/otAssert.h"

// Qt header
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qsplitter.h>
#include <QtWidgets/qgraphicsview.h>
#include <QtWidgets/qgraphicsscene.h>

namespace intern {
	enum navigationTreeColumns {
		ntTitle,
		ntCount
	};
}

ot::BlockPickerWidget::BlockPickerWidget(Qt::Orientation _orientation) : m_navigation(nullptr), m_splitter(nullptr), m_repaintPreviewRequired(false), m_previewSize(50, 50) {
	// Create controls
	m_splitter = new QSplitter(_orientation);
	
	m_navigation = new ot::TreeWidgetFilter;
	m_navigation->treeWidget()->setHeaderHidden(true);
	m_navigation->setWidgetFlags(ot::ApplyFilterOnTextChange);

	m_viewLayoutW = new QWidget;
	m_viewLayout = new QGridLayout(m_viewLayoutW);

	m_splitter->addWidget(m_navigation->getWidget());
	m_splitter->addWidget(m_viewLayoutW);

	connect(m_navigation->treeWidget(), &QTreeWidget::itemSelectionChanged, this, &BlockPickerWidget::slotSelectionChanged);
}

ot::BlockPickerWidget::~BlockPickerWidget() {

}

// ##############################################################################################################################

QWidget* ot::BlockPickerWidget::widget(void) {
	return m_splitter;
}

// ##############################################################################################################################

void ot::BlockPickerWidget::setOrientation(Qt::Orientation _orientation) {
	m_splitter->setOrientation(_orientation);
}

Qt::Orientation ot::BlockPickerWidget::orientation(void) const {
	return m_splitter->orientation();
}

void ot::BlockPickerWidget::addTopLevelCategory(ot::BlockCategoryConfiguration* _topLevelCategory) {
	this->addCategory(_topLevelCategory, nullptr);
}

void ot::BlockPickerWidget::addTopLevelCategories(const std::list<ot::BlockCategoryConfiguration*>& _topLevelCategories) {
	this->addCategories(_topLevelCategories, nullptr);
}

void ot::BlockPickerWidget::clear(void) {
	m_navigation->treeWidget()->clear();
	for (auto d : m_previewData) { 
		for (auto e : d.second) delete e;
	}
	m_previewData.clear();
	for (auto v : m_views) {
		m_viewLayout->removeWidget(v.view);
		delete v.view;
	}
	m_views.clear();
}

// ##############################################################################################################################

// Private: Slots

void ot::BlockPickerWidget::slotSelectionChanged(void) {
	for (auto v : m_views) {
		m_viewLayout->removeWidget(v.view);
		delete v.view;
	}
	m_views.clear();

	std::list<PreviewBox> previews;

	for (auto itm : m_navigation->treeWidget()->selectedItems()) {
		auto it = m_previewData.find(itm);
		if (it != m_previewData.end()) {
			for (auto bCfg : it->second) {
				// Construct block
				ot::Block* newBlock = BlockFactory::blockFromConfig(bCfg);
				newBlock->setBlockContextFlags(ot::BlockContextFlags(ot::PreviewBlockContext));

				if (newBlock) {
					//newBlock->setFlag(QGraphicsItem::ItemIsMovable, true);
					
					PreviewBox box;

					box.scene = new GraphicsScene;
					box.scene->setGridSize(0);

					box.view = new GraphicsView;
					box.view->setScene(box.scene);
					box.view->setMaximumSize(m_previewSize);
					box.view->setMinimumSize(m_previewSize);
					box.view->setDragMode(QGraphicsView::NoDrag);

					newBlock->finalize(box.scene);
					
					box.view->viewAll();

					m_viewLayout->addWidget(box.view);
					//m_viewLayout->addWidget(box.view, 0, 0);
					m_views.push_back(box);
				}
			}
		}
	}
}

// ##############################################################################################################################

// Private: Helper

void ot::BlockPickerWidget::addCategory(ot::BlockCategoryConfiguration* _category, QTreeWidgetItem* _parentNavigationItem) {
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

	this->addCategories(_category->childs(), categoryItem);
	this->addBlocksToNavigation(_category->items(), categoryItem);
}

void ot::BlockPickerWidget::addCategories(const std::list<ot::BlockCategoryConfiguration*>& _categories, QTreeWidgetItem* _parentNavigationItem) {
	for (auto c : _categories)
	{
		this->addCategory(c, _parentNavigationItem);
	}
}

void ot::BlockPickerWidget::addBlockToNavigation(ot::BlockConfiguration* _block, QTreeWidgetItem* _parentNavigationItem) {
	otAssert(_block, "nullptr provided");
	otAssert(_parentNavigationItem, "nullptr provided");

	QTreeWidgetItem* blockItem = nullptr;
	for (int i = 0; i < _parentNavigationItem->childCount(); i++) {
		if (_parentNavigationItem->child(i)->text(intern::ntTitle).toLower() == QString::fromStdString(_block->title()).toLower()) {
			blockItem = _parentNavigationItem->child(i);
			break;
		}
	}

	if (blockItem == nullptr) {
		blockItem = new QTreeWidgetItem;
		blockItem->setText(intern::ntTitle, QString::fromStdString(_block->title()));
		_parentNavigationItem->addChild(blockItem);
		blockItem->setHidden(true);

		OT_rJSON_createDOC(doc);
		OT_rJSON_createValueObject(obj);
		_block->addToJsonObject(doc, obj);
		storePreviewData(_parentNavigationItem, BlockConfigurationFactory::blockConfigurationFromJson(obj));
	}
}

void ot::BlockPickerWidget::addBlocksToNavigation(const std::list <ot::BlockConfiguration*>& _blocks, QTreeWidgetItem* _parentNavigationItem) {
	for (auto b : _blocks) {
		this->addBlockToNavigation(b, _parentNavigationItem);
	}
}

void ot::BlockPickerWidget::storePreviewData(QTreeWidgetItem* _item, BlockConfiguration* _config) {
	auto it = m_previewData.find(_item);
	if (it != m_previewData.end()) {
		it->second.push_back(_config);
	}
	else {
		m_previewData.insert_or_assign(_item, std::list<BlockConfiguration*>());
		storePreviewData(_item, _config);
	}

}