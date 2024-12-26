//! @file WidgetTest.h
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

#include "WidgetTest.h"

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/PerformanceTests.h"
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
#include <QtCore/qfile.h>
#include <QtWidgets/qfiledialog.h>
#include <QtWidgets/qmessagebox.h>
#include <QtWidgets/qtablewidget.h>

class TestToolBar : public QToolBar {
public:
	TestToolBar(WidgetTest* _owner) : m_owner(_owner) {

	}

public Q_SLOTS:
	void slotTest(void) {
		QString fileName = QFileDialog::getOpenFileName(this, "Open JSON file", "", "JSON (*.json)");
		if (fileName.isEmpty()) return;

		QFile file(fileName);
		if (!file.open(QIODevice::ReadOnly)) {
			QMessageBox msg(QMessageBox::Critical, "Error", "Failed to open file for reading", QMessageBox::Ok);
			msg.exec();
			return;
		}

		std::string str = file.readAll().toStdString();
		file.close();

		try {
			ot::JsonDocument result;
			if (result.fromJson(str)) {
				QMessageBox msg(QMessageBox::Information, "Ok", "Parse Ok", QMessageBox::Ok);
				msg.exec();
				return;
			}
			else {
				QMessageBox msg(QMessageBox::Warning, "Error", "Parse error", QMessageBox::Ok);
				msg.exec();
				return;
			}
		}
		catch (const std::exception& _e) {
			QMessageBox msg(QMessageBox::Critical, "Error", _e.what(), QMessageBox::Ok);
			msg.exec();
			return;
		}
		catch (...) {
			QMessageBox msg(QMessageBox::Critical, "Error", "Unknown error", QMessageBox::Ok);
			msg.exec();
			return;
		}
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

	this->updateVersionConfig("2.1.3", "2.1");

	root->addWidget(m_versionGraph->getQWidget());
	this->connect(m_versionGraph->getGraph(), &VersionGraph::versionDeselected, this, &WidgetTest::slotVersionDeselected);
	this->connect(m_versionGraph->getGraph(), &VersionGraph::versionSelected, this, &WidgetTest::slotVersionSelected);
	this->connect(m_versionGraph->getGraph(), &VersionGraph::versionActivateRequest, this, &WidgetTest::slotVersionActivatRequest);

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
	this->updateVersionConfig(_versionName, _versionName);
}

void WidgetTest::updateVersionConfig(const std::string& _activeVersionName, const std::string& _activeVersionBranch) {
	ot::VersionGraphCfg cfg;
	{
		ot::PerformanceIntervalTest test;

		std::list<std::list<ot::VersionGraphVersionCfg>> branches;
		std::list<ot::VersionGraphVersionCfg> b1;
		
		// 1 - 5
		for (int i = 1, par = 0; i < 6; i++, par++) {
			ot::VersionGraphVersionCfg v(std::to_string(i));
			if (par == 0) {
				v.setParentVersion("");
			}
			else {
				v.setParentVersion(std::to_string(par));
			}
			b1.push_back(std::move(v));
		}

		// 2.1.1 - 2.1.5
		std::list<ot::VersionGraphVersionCfg> b2;
		for (int i = 1, par = 0; i < 6; i++, par++) {
			ot::VersionGraphVersionCfg v("2.1." + std::to_string(i));
			if (par == 0) {
				v.setParentVersion("2");
			}
			else {
				v.setParentVersion("2.1." + std::to_string(par));
			}
			b2.push_back(std::move(v));
		}

		// 4.1.1 - 4.1.2
		std::list<ot::VersionGraphVersionCfg> b3;
		for (int i = 1, par = 0; i < 4; i++, par++) {
			ot::VersionGraphVersionCfg v("4.1." + std::to_string(i));
			if (par == 0) {
				v.setParentVersion("4");
			}
			else {
				v.setParentVersion("4.1." + std::to_string(par));
			}
			b3.push_back(std::move(v));
		}

		std::list<ot::VersionGraphVersionCfg> b4;
		std::list<ot::VersionGraphVersionCfg> b5;
		for (int i = 1, par = 0; i < 4; i++, par++) {
			ot::VersionGraphVersionCfg v("4.1.2.1." + std::to_string(i));
			ot::VersionGraphVersionCfg v2("4.1.2.2." + std::to_string(i));
			if (par == 0) {
				v.setParentVersion("4.1.2");
			}
			else {
				v.setParentVersion("4.1.2.1." + std::to_string(par));
				v2.setParentVersion("4.1.2.2." + std::to_string(par));
			}
			b4.push_back(std::move(v));
			b5.push_back(std::move(v2));
		}

		branches.push_back(b1);
		branches.push_back(b2);
		branches.push_back(b3);
		branches.push_back(b4);
		branches.push_back(b5);

		cfg.setBranches(branches);
	
		cfg.setActiveVersionName(_activeVersionName);
		cfg.setActiveBranchName(_activeVersionBranch);

		test.logCurrentInterval("Create config");
	}

	// Serialize
	std::string jsonString;
	{
		ot::PerformanceIntervalTest test;
		ot::JsonDocument doc;
		cfg.addToJsonObject(doc, doc.GetAllocator());

		jsonString = doc.toJson();

		test.logCurrentInterval("Serialize");
	}
	
	ot::VersionGraphCfg newCfg;
	{
		ot::PerformanceIntervalTest test;

		ot::JsonDocument newDoc;
		newDoc.fromJson(jsonString);
		newCfg.setFromJsonObject(newDoc.GetConstObject());

		test.logCurrentInterval("Deserialize");
	}

	{
		ot::PerformanceIntervalTest test;
		m_versionGraph->setupConfig(std::move(newCfg));
		test.logCurrentInterval("Setup graph");
	}
	
}
