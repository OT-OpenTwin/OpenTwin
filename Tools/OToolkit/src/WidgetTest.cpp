//! @file WidgetTest.h
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

#include "WidgetTest.h"

// OpenTwin header
#include "OTGui/NavigationTreeItem.h"
#include "OTGui/PropertyDialogCfg.h"
#include "OTCore/PropertyStringList.h"
#include "OTCore/PropertyGroup.h"
#include "OTWidgets/Splitter.h"
#include "OTWidgets/TreeWidget.h"
#include "OTWidgets/TreeWidgetFilter.h"
#include "OTWidgets/PropertyDialog.h"
#include "OTWidgets/PropertyGridGroup.h"

// Qt header
#include <QtWidgets/qtablewidget.h>

class TestToolBar : public QToolBar {
public:
	TestToolBar(WidgetTest* _owner) : m_owner(_owner) {

	}

public Q_SLOTS:
	void slotTest(void) {
		ot::PropertyDialogCfg cfg;
		ot::PropertyGroup* g1 = new ot::PropertyGroup("General");
		ot::PropertyGroup* g11 = new ot::PropertyGroup("Appearance");

		ot::PropertyStringList* t1 = new ot::PropertyStringList("ColorStyle", "Bright", std::list<std::string>{ "Bright", "Dark" });
		g11->addProperty(t1);
		g1->addChildGroup(g11);
		cfg.addRootGroup(g1);
		ot::PropertyDialog dia(cfg);
		dia.showDialog();
	}

private:
	WidgetTest* m_owner;
};

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

	TestToolBar* test = new TestToolBar(this);
	QAction* testAction = test->addAction("Test");
	test->connect(testAction, &QAction::triggered, test, &TestToolBar::slotTest);
	_content.setToolBar(test);

	return true;
}