//! @file WidgetTest.h
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

#include "WidgetTest.h"

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTCore/RuntimeTests.h"

#include "OTGui/PropertyGroup.h"
#include "OTGui/PropertyDialogCfg.h"
#include "OTGui/PropertyDialogCfg.h"
#include "OTGui/NavigationTreeItem.h"
#include "OTGui/PropertyStringList.h"
#include "OTGui/NavigationTreeItem.h"
#include "OTGui/PropertyStringList.h"

#include "OTWidgets/Splitter.h"
#include "OTWidgets/TabToolBar.h"
#include "OTWidgets/TextEditor.h"
#include "OTWidgets/TreeWidget.h"
#include "OTWidgets/MainWindow.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/PropertyDialog.h"
#include "OTWidgets/TextEditorView.h"
#include "OTWidgets/StatusBarManager.h"
#include "OTWidgets/TreeWidgetFilter.h"
#include "OTWidgets/PropertyGridGroup.h"
#include "OTWidgets/VersionGraphManager.h"
#include "OTWidgets/CentralWidgetManager.h"

// Qt header
#include <QtCore/qfile.h>
#include <QtWidgets/qfiledialog.h>
#include <QtWidgets/qmessagebox.h>
#include <QtWidgets/qtablewidget.h>

class TestToolBar : public QToolBar {
public:
	TestToolBar(WidgetTest* _owner) :
		m_owner(_owner), m_window(nullptr), m_centralWidgetManager(nullptr), m_toolBarManager(nullptr), m_statusBarManager(nullptr)
	{

	}

public Q_SLOTS:
	void slotTest(void) {
		using namespace ot;

		if (m_window) {
			OT_LOG_EA("Main window already created");
		}
		m_window = new MainWindow;

		m_centralWidgetManager = new CentralWidgetManager;
		
		m_centralWidgetManager->setOverlayAnimation(IconManager::getMovie("Animations/OpenTwinLoading.gif"));

		m_window->setCentralWidget(m_centralWidgetManager);

		m_toolBarManager = new TabToolBar;


		m_statusBarManager = new StatusBarManager;
	}

private:
	WidgetTest* m_owner;

	ot::MainWindow* m_window;
	ot::CentralWidgetManager* m_centralWidgetManager;
	ot::TabToolBar* m_toolBarManager;
	ot::StatusBarManager* m_statusBarManager;
};

bool WidgetTest::runTool(QMenu* _rootMenu, otoolkit::ToolWidgets& _content) {
	using namespace ot;
	
	{
		Splitter* root = new Splitter;
		ot::WidgetView* r = this->createCentralWidgetView(root, "Test Versions");
		_content.addView(r);

		m_versionGraph = new ot::VersionGraphManager;
		m_versionGraph->setCurrentViewMode(ot::VersionGraphManager::Iterator);

		this->updateVersionConfig(ot::VersionGraphVersionCfg("2.1.3"));

		root->addWidget(m_versionGraph->getQWidget());
		this->connect(m_versionGraph->getGraph(), &VersionGraph::versionDeselected, this, &WidgetTest::slotVersionDeselected);
		this->connect(m_versionGraph->getGraph(), &VersionGraph::versionSelected, this, &WidgetTest::slotVersionSelected);
		this->connect(m_versionGraph->getGraph(), &VersionGraph::versionActivateRequest, this, &WidgetTest::slotVersionActivatRequest);
	}
	{
		ot::TextEditor* editor = new TextEditor;
		TextEditorCfg cfg;
		cfg.setEntityName("Test Editor");
		cfg.setTitle("Test Editor");
		cfg.setDocumentSyntax(DocumentSyntax::PythonScript);
		cfg.setPlainText(
			"def test():\n"
			"    print('Hello World')\n"
			"    print(\"Hello\")\n"
			"\n"
			"test()\n"
			"class TestClass:\n"
			"    def __init__(self):\n"
			"        self.value = 42\n"
			"\n"
			"# This is a comment\n"
		);

		editor->setupFromConfig(cfg, false);
		_content.addView(this->createCentralWidgetView(editor, "Test Editor"));
	}
	{
		QTableWidget* table = new QTableWidget(2, 2);
		ot::WidgetView* r = this->createCentralWidgetView(table, "Test Table");
		_content.addView(r);

		table->setItem(0, 0, new QTableWidgetItem("Test00"));
		table->setItem(0, 1, new QTableWidgetItem("Test01"));
		table->setItem(1, 0, new QTableWidgetItem("Test10"));
		table->setItem(1, 1, new QTableWidgetItem("Test11"));

		table->setSortingEnabled(true);
	}

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
	ot::VersionGraphVersionCfg version(_versionName);
	this->updateVersionConfig(version);
}

void WidgetTest::updateVersionConfig(const ot::VersionGraphVersionCfg& _version) {
	OT_TEST_Interval(totalTest, "UpdateVersion", "Total time");

	ot::VersionGraphCfg cfg;
	cfg.setActiveVersionName(_version.getName());
	cfg.setActiveBranchName(_version.getBranchName());
	{
		OT_TEST_Interval(test, "UpdateVersion", "Create config");

		std::list<ot::VersionGraphVersionCfg> currentBranch;
		
		// 1 - 5
		for (int i = 1, par = 0; i < 6; i++, par++) {
			ot::VersionGraphVersionCfg v(std::to_string(i));
			if (par == 0) {
				v.setParentVersion("");
			}
			else {
				v.setParentVersion(std::to_string(par));
			}
			currentBranch.push_back(std::move(v));
		}
		if (!currentBranch.empty()) {
			cfg.insertBranch(std::move(currentBranch));
		}

		// 2.1.1 - 2.1.5
		for (int i = 1, par = 0; i < 6; i++, par++) {
			ot::VersionGraphVersionCfg v("2.1." + std::to_string(i));
			if (par == 0) {
				v.setParentVersion("2");
			}
			else {
				v.setParentVersion("2.1." + std::to_string(par));
			}
			currentBranch.push_back(std::move(v));
		}
		if (!currentBranch.empty()) {
			cfg.insertBranch(std::move(currentBranch));
		}

		// 4.1.1 - 4.1.2
		for (int i = 1, par = 0; i < 4; i++, par++) {
			ot::VersionGraphVersionCfg v("4.1." + std::to_string(i));
			if (par == 0) {
				v.setParentVersion("4");
			}
			else {
				v.setParentVersion("4.1." + std::to_string(par));
			}
			currentBranch.push_back(std::move(v));
		}
		if (!currentBranch.empty()) {
			cfg.insertBranch(std::move(currentBranch));
		}

		std::list<ot::VersionGraphVersionCfg> currentBranchA;
		std::list<ot::VersionGraphVersionCfg> currentBranchB;
		std::list<ot::VersionGraphVersionCfg> currentBranchC;
		std::list<ot::VersionGraphVersionCfg> currentBranchD;
		std::list<ot::VersionGraphVersionCfg> currentBranchE;
		
		for (int i = 1, par = 0; i < 20000; i++, par++) {
			ot::VersionGraphVersionCfg a("4.1.2.1." + std::to_string(i));
			ot::VersionGraphVersionCfg b("4.1.2.2." + std::to_string(i));
			ot::VersionGraphVersionCfg c("4.1.2.3." + std::to_string(i));
			ot::VersionGraphVersionCfg d("4.1.2.4." + std::to_string(i));
			ot::VersionGraphVersionCfg e("4.1.2.5." + std::to_string(i));
			if (par == 0) {
				a.setParentVersion("4.1.2");
				b.setParentVersion("4.1.2");
				c.setParentVersion("4.1.2");
				d.setParentVersion("4.1.2");
				e.setParentVersion("4.1.2");
			}
			else {
				a.setParentVersion("4.1.2.1." + std::to_string(par));
				b.setParentVersion("4.1.2.2." + std::to_string(par));
				c.setParentVersion("4.1.2.3." + std::to_string(par));
				d.setParentVersion("4.1.2.4." + std::to_string(par));
				e.setParentVersion("4.1.2.5." + std::to_string(par));
			}
			currentBranchA.push_back(std::move(a));
			currentBranchB.push_back(std::move(b));
			currentBranchC.push_back(std::move(c));
			currentBranchD.push_back(std::move(d));
			currentBranchE.push_back(std::move(e));
		}
		if (!currentBranchE.empty()) {
			cfg.insertBranch(std::move(currentBranchE));
		}
		if (!currentBranchD.empty()) {
			cfg.insertBranch(std::move(currentBranchD));
		}
		if (!currentBranchC.empty()) {
			cfg.insertBranch(std::move(currentBranchC));
		}
		if (!currentBranchB.empty()) {
			cfg.insertBranch(std::move(currentBranchB));
		}
		if (!currentBranchA.empty()) {
			cfg.insertBranch(std::move(currentBranchA));
		}
	}

	// Serialize
	std::string jsonString;
	{
		OT_TEST_Interval(test, "UpdateVersion", "Serialize");
		ot::JsonDocument doc;
		cfg.addToJsonObject(doc, doc.GetAllocator());

		jsonString = doc.toJson();
	}
	
	ot::VersionGraphCfg newCfg;
	{
		OT_TEST_Interval(test, "UpdateVersion", "Deserialize");

		ot::JsonDocument newDoc;
		newDoc.fromJson(jsonString);
		newCfg.setFromJsonObject(newDoc.getConstObject());
	}

	{
		OT_TEST_Interval(test, "UpdateVersion", "Setup graph");
		m_versionGraph->setupConfig(std::move(newCfg));
	}
}
