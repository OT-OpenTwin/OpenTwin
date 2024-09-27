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

	this->updateVersionConfig("1.1.5", "1.1.3");

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
	this->updateVersionConfig(_versionName, "1.1.3");
}

void WidgetTest::updateVersionConfig(const std::string& _activeVersionName, const std::string& _activeVersionBranch) {
	ot::VersionGraphCfg cfg;
	cfg.setRootVersion("1", "Initial commit");
	
	cfg.getRootVersion()->addChildVersion("2")->addChildVersion("3");
	ot::VersionGraphVersionCfg* branchItem = cfg.getRootVersion()
		->addChildVersion("1.1.1", "Done some longer work on this item so we have to check the title")
		->addChildVersion("1.1.2");

	branchItem->addChildVersion("1.1.3")
		->addChildVersion("1.1.4")
		->addChildVersion("1.1.5")
		->addChildVersion("1.1.6")
		->addChildVersion("1.1.7")
		->addChildVersion("1.1.8", "testing label filter mode")
		->addChildVersion("1.1.9")
		->addChildVersion("1.1.10")
		->addChildVersion("1.1.11");

	branchItem->addChildVersion("1.1.2.1.1")
		->addChildVersion("1.1.2.1.2")
		->addChildVersion("1.1.2.1.3");

	cfg.setActiveVersionName(_activeVersionName);
	cfg.setActiveBranchVersionName(_activeVersionBranch);

	m_versionGraph->setupConfig(cfg);
}
