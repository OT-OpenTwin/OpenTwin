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
	ot::VersionGraphVersionCfg v24("2.4");
	ot::VersionGraphVersionCfg v25("2.5");
	ot::VersionGraphVersionCfg v26("2.6");
	ot::VersionGraphVersionCfg v27("2.7");
	ot::VersionGraphVersionCfg v28("2.8");
	ot::VersionGraphVersionCfg v29("2.9", "testing label filter mode");
	ot::VersionGraphVersionCfg v210("2.10");
	ot::VersionGraphVersionCfg v211("2.11");
	ot::VersionGraphVersionCfg v212("2.12");
	ot::VersionGraphVersionCfg v33("3.3");
	
	v211.addChildVersion(v212);
	v210.addChildVersion(v211);
	v29.addChildVersion(v210);
	v28.addChildVersion(v29);
	v27.addChildVersion(v28);
	v26.addChildVersion(v27);
	v25.addChildVersion(v26);
	v24.addChildVersion(v25);
	v23.addChildVersion(v24);
	v22.addChildVersion(v23);
	v22.addChildVersion(v33);
	v1.addChildVersion(v12);
	v1.addChildVersion(v22);
	cfg.addRootVersion(v1);
	cfg.setActiveVersionName("2.6");

	m_versionGraph->setupConfig(cfg);
	root->addWidget(m_versionGraph->getQWidget());

	TestToolBar* test = new TestToolBar(this);
	QAction* testAction = test->addAction("Test");
	test->connect(testAction, &QAction::triggered, test, &TestToolBar::slotTest);
	_content.setToolBar(test);

	return true;
}
