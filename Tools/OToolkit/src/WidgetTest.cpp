//! @file WidgetTest.h
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

#include "WidgetTest.h"

// OpenTwin header
#include "OTGui/NavigationTreeItem.h"
#include "OTWidgets/Splitter.h"
#include "OTWidgets/TreeWidget.h"
#include "OTWidgets/TreeWidgetFilter.h"

// Qt header
#include <QtWidgets/qtablewidget.h>

bool WidgetTest::runTool(QMenu* _rootMenu, otoolkit::ToolWidgets& _content) {
	using namespace ot;
	Splitter* root = new Splitter;

	TreeWidgetFilter* tree = new TreeWidgetFilter;
	tree->treeWidget()->setAlternatingRowColors(true);
	tree->treeWidget()->setSelectionMode(QAbstractItemView::MultiSelection);
	root->addWidget(tree->getQWidget());
	{
		NavigationTreeItem itm1("Test A");
		itm1.addChildItem(NavigationTreeItem("1"));
		itm1.addChildItem(NavigationTreeItem("2"));
		itm1.addChildItem(NavigationTreeItem("3"));
		NavigationTreeItem itm2("Test B");
		itm2.addChildItem(NavigationTreeItem("4"));
		itm2.addChildItem(NavigationTreeItem("5"));
		itm2.addChildItem(NavigationTreeItem("6"));

		tree->treeWidget()->addItem(TreeWidgetItemInfo(itm1));
		tree->treeWidget()->addItem(TreeWidgetItemInfo(itm2));
	}

	QTableWidget* table = new QTableWidget(3, 2);
	table->setVerticalHeaderLabels({"R1", "R2", "R3"});
	table->setHorizontalHeaderLabels({"A", "B"});
	root->addWidget(table);

	_content.setRootWidget(root);
	return true;
}