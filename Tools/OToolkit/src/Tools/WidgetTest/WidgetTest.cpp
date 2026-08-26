// @otlicense
// File: WidgetTest.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#include "Tools/WidgetTest/WidgetTest.h"
#include "Tools/WidgetTest/TestToolBar.h"
#include "Widgets/AdvancedJsonTree.h"

// OpenTwin header
#include "OTCore/Logging/Logger.h"
#include "OTCore/Debugging/RuntimeTests.h"

#include "OTGui/Dialog/PropertyDialogCfg.h"
#include "OTGui/Properties/PropertyGroup.h"
#include "OTGui/Properties/PropertyStringList.h"
#include "OTGui/Widgets/NavigationTreeItemCfg.h"

#include "OTWidgets/ToolBar/TabToolBar.h"
#include "OTWidgets/Version/VersionGraphManager.h"
#include "OTWidgets/Widgets/Splitter.h"
#include "OTWidgets/Widgets/MainWindow.h"
#include "OTWidgets/Widgets/JsonTreeWidget.h"
#include "OTWidgets/Widgets/StyledSvgWidget.h"
#include "OTWidgets/WidgetView/TableView.h"
#include "OTWidgets/WidgetView/TextEditorView.h"
#include "OTWidgets/WidgetView/WidgetViewManager.h"
#include "OTWidgets/WindowManagement/CentralWidgetManager.h"

// Qt header
#include <QtCore/qfile.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qfiledialog.h>
#include <QtWidgets/qmessagebox.h>
#include <QtWidgets/qtablewidget.h>

namespace intern
{
	namespace testdata
	{

		static ot::JsonDocument createTestJsonDocument()
		{
			using namespace ot;

			JsonDocument doc;
			JsonAllocator& alloc = doc.GetAllocator();

			JsonObject obj1;
			obj1.AddMember("1.1", JsonString("111", alloc), alloc);
			obj1.AddMember("1.2", JsonString("222", alloc), alloc);
			obj1.AddMember("1.3", std::numeric_limits<size_t>::max() - 1, alloc);
			obj1.AddMember("1.4", true, alloc);

			JsonArray arr15;
			arr15.PushBack(JsonString("1.5.1", alloc), alloc);
			arr15.PushBack(JsonString("1.5.2", alloc), alloc);
			arr15.PushBack(JsonString("1.5.3", alloc), alloc);
			obj1.AddMember("1.5", arr15, alloc);
			doc.AddMember("1", obj1, alloc);

			JsonObject objA;
			JsonArray arrAa;
			
			JsonObject objAaA;
			objAaA.AddMember("A.A.A.A", JsonString("A.A.A.A.A", alloc), alloc);
			objAaA.AddMember("A.A.A.B", JsonString("A.A.A.B.A", alloc), alloc);
			arrAa.PushBack(objAaA, alloc);

			objA.AddMember("A.A", arrAa, alloc);
			objA.AddMember("A.B", JsonString("A.B.A", alloc), alloc);
			objA.AddMember("A.C", false, alloc);

			JsonArray arrAD;
			for (char i = 'A'; i <= 'H'; i++)
			{
				std::string ix = { 'A', '.', 'D', '.', i };

				JsonArray arrNest;
				for (char j = 'A'; j <= 'Z'; j++)
				{
					std::string name(ix);
					name.append({ '.', j });

					arrNest.PushBack(JsonString(name, alloc), alloc);
				}
				arrAD.PushBack(arrNest, alloc);
			}

			objA.AddMember("A.D", arrAD, alloc);
			objA.AddMember("A.E", 3.14159, alloc);
			objA.AddMember("A.F", std::numeric_limits<int>::lowest(), alloc);
			objA.AddMember("A.G", std::numeric_limits<int>::max(), alloc);
			doc.AddMember("A", objA, alloc);

			JsonObject objB;
			for (int i = 1; i <= 3; i++)
			{
				std::string nameB1 = "B.";
				nameB1.append(std::to_string(i));
				
				JsonArray arrB1;
				for (char j = 'a'; j <= 'z'; j++)
				{
					std::string nameB1A(nameB1);
					nameB1A.append(".");
					nameB1A.push_back(j);

					JsonObject objB1A;
					for (char k = 'A'; k <= 'Z'; k++)
					{
						std::string nameB1A1(nameB1A);
						nameB1A1.append(".");
						nameB1A1.push_back(k);
						JsonArray arrB1A1;
						for (char l = '1'; l < '4'; l++)
						{
							std::string nameB1A1i(nameB1A1);
							nameB1A1i.append(".");
							nameB1A1i.append(5, l);
							arrB1A1.PushBack(JsonString(nameB1A1i, alloc), alloc);
						}
						objB1A.AddMember(JsonString(nameB1A1, alloc), arrB1A1, alloc);

					}
					arrB1.PushBack(objB1A, alloc);
				}
				
				objB.AddMember(JsonString(nameB1, alloc), arrB1, alloc);
			}
			doc.AddMember("B", objB, alloc);

			return doc;
		}

	}
}

bool WidgetTest::runTool(QMenu* _rootMenu, otoolkit::ToolWidgets& _content) {
	using namespace ot;
	
	if (false) {
		Splitter* root = new Splitter(nullptr);
		ot::WidgetView* r = this->createCentralWidgetView(root, "Test Versions");
		_content.addView(r);

		m_versionGraph = new ot::VersionGraphManager(root);
		m_versionGraph->setCurrentViewMode(ot::VersionGraphManager::Iterator);

		this->updateVersionConfig(ot::VersionGraphVersionCfg("2.1.3"));

		root->addWidget(m_versionGraph->getQWidget());
		this->connect(m_versionGraph->getGraph(), &VersionGraph::versionDeselected, this, &WidgetTest::slotVersionDeselected);
		this->connect(m_versionGraph->getGraph(), &VersionGraph::versionSelected, this, &WidgetTest::slotVersionSelected);
		this->connect(m_versionGraph->getGraph(), &VersionGraph::versionActivateRequest, this, &WidgetTest::slotVersionActivatRequest);
	}
	
	if (false) {
		ot::TextEditor* editor = new TextEditor(nullptr);
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
	
	if (false) {
		QTableWidget* table = new QTableWidget(2, 2);
		ot::WidgetView* r = this->createCentralWidgetView(table, "Test Table");
		_content.addView(r);

		table->setItem(0, 0, new QTableWidgetItem("Test00"));
		table->setItem(0, 1, new QTableWidgetItem("Test01"));
		table->setItem(1, 0, new QTableWidgetItem("Test10"));
		table->setItem(1, 1, new QTableWidgetItem("Test11"));

		table->setSortingEnabled(true);
	}

	if (false) {
		QWidget* w = new QWidget;
		QGridLayout* l = new QGridLayout(w);
		ot::StyledSvgWidget* svg = new ot::StyledSvgWidget("Application/OpenTwinBackground.svg", w);
		svg->setMaintainAspectRatio(true);
		svg->setFixedSize(512, 512);
		l->addWidget(svg, 1, 1);
		l->setColumnStretch(0, 1);
		l->setColumnStretch(2, 1);
		l->setRowStretch(0, 1);
		l->setRowStretch(2, 1);

		_content.addView(this->createCentralWidgetView(w, "Test Logo"));
	}

	if (false) {
		WidgetViewManager* man = new WidgetViewManager;
		man->initialize();

		TextEditorView* txt = new TextEditorView(man->getDockManager());
		WidgetViewBase txtBase(WidgetViewBase::ViewText, WidgetViewBase::ViewIsCentral);
		txtBase.setEntityName("Txt test");
		txt->setViewData(txtBase);
		man->addView(BasicServiceInformation(), txt);

		TableView* table = new TableView(man->getDockManager());
		WidgetViewBase tableBase(WidgetViewBase::ViewTable, WidgetViewBase::ViewIsCentral);
		tableBase.setEntityName("Table test");
		table->setViewData(tableBase);
		man->addView(BasicServiceInformation(), table);

		_content.addView(this->createCentralWidgetView(man->getDockManager(), "Test Container"));
	}

	if (true)
	{
		AdvancedJsonTree* advJsonTree = new AdvancedJsonTree(nullptr);
		_content.addView(this->createCentralWidgetView(advJsonTree, "Test Advanced JSON Tree"));
		advJsonTree->setFromJsonDocument(intern::testdata::createTestJsonDocument());
	}

	if (false) {
		JsonTreeWidget* jsonTree = new JsonTreeWidget(nullptr);
		_content.addView(this->createCentralWidgetView(jsonTree, "Test JSON Tree"));
		jsonTree->setFromJsonDocument(intern::testdata::createTestJsonDocument());
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
	ot::RuntimeIntervalTestLog  totalTest("[UpdateVersion] Total time");

	ot::VersionGraphCfg cfg;
	cfg.setActiveVersionName(_version.getName());
	cfg.setActiveBranchName(_version.getBranchName());
	{
		ot::RuntimeIntervalTestLog  test("[UpdateVersion] Create config");

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
		ot::RuntimeIntervalTestLog serializeTest("[UpdateVersion] Serialize");
		ot::JsonDocument doc;
		cfg.addToJsonObject(doc, doc.GetAllocator());

		jsonString = doc.toJson();
	}
	
	ot::VersionGraphCfg newCfg;
	{
		ot::RuntimeIntervalTestLog deserializeTest("[UpdateVersion] Deserialize");
		ot::JsonDocument newDoc;
		newDoc.fromJson(jsonString);
		newCfg.setFromJsonObject(newDoc.getConstObject());
	}

	{
		ot::RuntimeIntervalTestLog deserializeTest("[UpdateVersion] Setup graph");
		m_versionGraph->setupConfig(std::move(newCfg));
	}
}
