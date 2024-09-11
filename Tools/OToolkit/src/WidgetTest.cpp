//! @file WidgetTest.h
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

#include "WidgetTest.h"

// OpenTwin header
#include "OTGui/NavigationTreeItem.h"
#include "OTGui/PropertyDialogCfg.h"
#include "OTGui/PropertyStringList.h"
#include "OTGui/PropertyGroup.h"
#include "OTWidgets/Splitter.h"
#include "OTWidgets/TextEditor.h"
#include "OTWidgets/TreeWidget.h"
#include "OTWidgets/TreeWidgetFilter.h"
#include "OTWidgets/PropertyDialog.h"
#include "OTWidgets/PropertyGridGroup.h"
#include "OTWidgets/VersionGraphManager.h"

// Qt header
#include <QtWidgets/qtablewidget.h>

class TestToolBar : public QToolBar {
public:
	TestToolBar(WidgetTest* _owner) : m_owner(_owner) {

	}

public Q_SLOTS:
	void slotTest(void) {

	}

private:
	WidgetTest* m_owner;
};

bool WidgetTest::runTool(QMenu* _rootMenu, otoolkit::ToolWidgets& _content) {
	using namespace ot;
	
	Splitter* root = new Splitter;
	ot::WidgetView* r = this->createCentralWidgetView(root, "Test");
	_content.addView(r);

	m_versionGraph = new ot::VersionGraphManager;

	ot::VersionGraphCfg cfg;
	ot::VersionGraphVersionCfg v1("1", "Initial commit");
	ot::VersionGraphVersionCfg v12("1.2");
	ot::VersionGraphVersionCfg v22("2.2", "Done some longer work on this item so we have to check the title");
	ot::VersionGraphVersionCfg v23("2.3");
	ot::VersionGraphVersionCfg v33("3.3");

	v22.addChildVersion(v23);
	v22.addChildVersion(v33);
	v1.addChildVersion(v12);
	v1.addChildVersion(v22);
	cfg.addRootVersion(v1);
	cfg.setActiveVersionName("2.3");

	m_versionGraph->setupConfig(cfg);
	root->addWidget(m_versionGraph->getQWidget());

	TestToolBar* test = new TestToolBar(this);
	QAction* testAction = test->addAction("Test");
	test->connect(testAction, &QAction::triggered, test, &TestToolBar::slotTest);
	_content.setToolBar(test);

	return true;
}
