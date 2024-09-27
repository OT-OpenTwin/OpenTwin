//! @file WidgetTest.h
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

#include "WidgetTest.h"

// OpenTwin header
#include "OTCore/Logger.h"
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

	this->updateVersionConfig("1.1.5", "1.1.2");

	root->addWidget(m_versionGraph->getQWidget());
	this->connect(m_versionGraph->getGraph(), &VersionGraph::versionDeselected, this, &WidgetTest::slotVersionDeselected);
	this->connect(m_versionGraph->getGraph(), &VersionGraph::versionSelected, this, &WidgetTest::slotVersionSelected);
	this->connect(m_versionGraph->getGraph(), &VersionGraph::versionActivatRequest, this, &WidgetTest::slotVersionActivatRequest);

	TestToolBar* test = new TestToolBar(this);
	QAction* testAction = test->addAction("Test");
	test->connect(testAction, &QAction::triggered, test, &TestToolBar::slotTest);
	_content.setToolBar(test);

	return true;
}

void WidgetTest::slotVersionDeselected(void) {
	OT_LOG_D("Version deselected");
}

void WidgetTest::slotVersionSelected(const std::string& _versionName) {
	OT_LOG_D("Version selected: " + _versionName);
}

void WidgetTest::slotVersionActivatRequest(const std::string& _versionName) {
	OT_LOG_D("Version activate request: " + _versionName);
	this->updateVersionConfig(_versionName, "1.1.2");
}

void WidgetTest::updateVersionConfig(const std::string& _activeVersionName, const std::string& _activeVersionBranch) {
	ot::VersionGraphCfg cfg;
	ot::VersionGraphVersionCfg v1("1", "Initial commit");
	ot::VersionGraphVersionCfg v2("2");
	ot::VersionGraphVersionCfg v111("1.1.1", "Done some longer work on this item so we have to check the title");
	ot::VersionGraphVersionCfg v112("1.1.2");
	ot::VersionGraphVersionCfg v113("1.1.3");
	ot::VersionGraphVersionCfg v114("1.1.4");
	ot::VersionGraphVersionCfg v115("1.1.5");
	ot::VersionGraphVersionCfg v116("1.1.6");
	ot::VersionGraphVersionCfg v117("1.1.7");
	ot::VersionGraphVersionCfg v118("1.1.8", "testing label filter mode");
	ot::VersionGraphVersionCfg v119("1.1.9");
	ot::VersionGraphVersionCfg v1110("1.1.10");
	ot::VersionGraphVersionCfg v1111("1.1.11");
	ot::VersionGraphVersionCfg v11211("1.1.1.1.1");

	v1110.addChildVersion(v1111);
	v119.addChildVersion(v1110);
	v118.addChildVersion(v119);
	v117.addChildVersion(v118);
	v116.addChildVersion(v117);
	v115.addChildVersion(v116);
	v114.addChildVersion(v115);
	v113.addChildVersion(v114);
	v112.addChildVersion(v113);

	v111.addChildVersion(v112);
	v111.addChildVersion(v11211);

	v1.addChildVersion(v2);
	v1.addChildVersion(v111);
	cfg.addRootVersion(v1);
	cfg.setActiveVersionName(_activeVersionName);
	cfg.setActiveBranchVersionName(_activeVersionBranch);

	m_versionGraph->setupConfig(cfg);
}
