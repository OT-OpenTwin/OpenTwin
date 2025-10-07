//! @file Application.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

// DebugService header
#include "Application.h"

// OpenTwin header
#include "OTCore/FolderNames.h"
#include "OTCore/RuntimeTests.h"
#include "OTCore/ReturnMessage.h"
#include "OTCore/ThisComputerInfo.h"

#include "OTGui/TableCfg.h"
#include "OTGui/PropertyInt.h"
#include "OTGui/PropertyBool.h"
#include "OTGui/PropertyColor.h"
#include "OTGui/PropertyGroup.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/PropertyString.h"
#include "OTGui/PropertyDouble.h"
#include "OTGui/PropertyGridCfg.h"
#include "OTGui/PropertyPainter2D.h"
#include "OTGui/PainterRainbowIterator.h"

#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"

#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTServiceFoundation/AbstractUiNotifier.h"
#include "OTServiceFoundation/AbstractModelNotifier.h"

#include "PlotBuilder.h"
#include "TemplateDefaultManager.h"
#include "ResultCollectionExtender.h"
#include "QuantityDescriptionCurve.h"

// std header
#include <thread>

#define OT_DEBUG_SERVICE_PAGE_NAME "Debug"

void testPropertyGrid() {
	using namespace ot;

	std::thread t([]() {
		auto uiComp = Application::instance().getUiComponent();

		JsonDocument fillDoc;
		fillDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_FillPropertyGrid, fillDoc.GetAllocator()), fillDoc.GetAllocator());
		PropertyGridCfg cfg;
		PropertyGroup* g1 = new PropertyGroup("Group 1");

		for (int i = 1; i <= 100; i++) {
			g1->addProperty(new PropertyBool("Bool " + std::to_string(i), (i % 2) == 0));
		}

		cfg.addRootGroup(g1);

		fillDoc.AddMember(OT_ACTION_PARAM_Config, JsonObject(cfg, fillDoc.GetAllocator()), fillDoc.GetAllocator());

		PropertyGridCfg emptyCfg;
		JsonDocument clearDoc;
		clearDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_FillPropertyGrid, clearDoc.GetAllocator()), clearDoc.GetAllocator());
		clearDoc.AddMember(OT_ACTION_PARAM_Config, JsonObject(emptyCfg, clearDoc.GetAllocator()), clearDoc.GetAllocator());

		for (int i = 0; i < 10000; i++) {
			std::string resp;
			uiComp->sendMessage(true, fillDoc, resp);
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			uiComp->sendMessage(true, clearDoc, resp);
			if (i % 100 == 0) {
				uiComp->displayMessage("Iteration: " + std::to_string(i + 1) + "\n");
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		uiComp->displayMessage("\nDONE\n");
		}
	);
	t.detach();
}

void Application::testCode() {
	testPropertyGrid();
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

Application::Application() :
	ot::ApplicationBase(OT_INFO_SERVICE_TYPE_DebugService, OT_INFO_SERVICE_TYPE_DebugService, new ot::AbstractUiNotifier(), new ot::AbstractModelNotifier()),
	m_nameCounter(0)
{
	// Debug buttons
	m_testButtons.push_back(ButtonInfo("Test", "Test", "BugRed", std::bind(&Application::testCode, this)));
	m_testButtons.push_back(ButtonInfo("Test", "Frontend Info", "Information", std::bind(&Application::uiDebugInfo, this)));
	m_testButtons.push_back(ButtonInfo("Test", "Debug Service Info", "Information", std::bind(&Application::serviceDebugInfo, this)));
	m_testButtons.push_back(ButtonInfo("Test", "Kill", "Kill", std::bind(&Application::testKill, this)));
	
	// Table tests
	m_testButtons.push_back(ButtonInfo("Table", "Small (100k)", "GreenCircle", std::bind(&Application::testTableSmall, this)));
	m_testButtons.push_back(ButtonInfo("Table", "Medium (1M)", "YellowCircle", std::bind(&Application::testTableMedium, this)));
	m_testButtons.push_back(ButtonInfo("Table", "Big (10M)", "RedCircle", std::bind(&Application::testTableBig, this)));

	// Plot tests
	m_testButtons.push_back(ButtonInfo("Plots", "Single Curve", "Plot1DVisible", std::bind(&Application::createPlotOneCurve, this)));
	m_testButtons.push_back(ButtonInfo("Plots", "Two Curves", "Plot1DVisible", std::bind(&Application::createPlotTwoCurves, this)));
	m_testButtons.push_back(ButtonInfo("Plots", "Family of Curves", "Plot1DVisible", std::bind(&Application::createFamilyOfCurves, this)));
	m_testButtons.push_back(ButtonInfo("Plots", "Family of Curves 3P const", "Plot1DVisible", std::bind(&Application::createFamilyOfCurves3ParameterConst, this)));
	m_testButtons.push_back(ButtonInfo("Plots", "Family of Curves 3P", "Plot1DVisible", std::bind(&Application::createFamilyOfCurves3Parameter, this)));
	m_testButtons.push_back(ButtonInfo("Plots", "Scatter Plot", "Plot1DVisible", std::bind(&Application::createPlotScatter, this)));
	m_testButtons.push_back(ButtonInfo("Plots", "Single Value Curve", "Plot1DVisible", std::bind(&Application::createPlotSinglePoint, this)));

	// --------------------------------------------------------------------------------------------------------+

	// Enable features (Exit)
	
	//this->enableFeature(DebugServiceConfig::ExitOnCreation);
	//this->enableFeature(DebugServiceConfig::ExitOnInit);
	//this->enableFeature(DebugServiceConfig::ExitOnPing);
	//this->enableFeature(DebugServiceConfig::ExitOnRun);
	//this->enableFeature(DebugServiceConfig::ExitOnPreShutdown);
		
	// Enable features (Other)

	//this->enableFeature(DebugServiceConfig::ExportOnStart);

	// --------------------------------------------------------------------------------------------------------+

	// If exit on init is enabled, exit the service
	if (this->getFeatureEnabled(DebugServiceConfig::ExitOnCreation)) {
		OT_LOG_T("Performing exit on creation");
		exit(0);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Button callbacks

void Application::uiDebugInfo() {
	using namespace ot;

	auto ui = this->getUiComponent();
	if (!ui) {
		OT_LOG_E("No ui? How?");
		return;
	}

	// Create table document
	JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_GetDebugInformation, doc.GetAllocator()), doc.GetAllocator());

	std::string resp;
	ui->sendMessage(true, doc, resp);
}

void Application::serviceDebugInfo(void) {
	getUiComponent()->displayMessage(
		"Hello :-)\n\nThis Computer Info (Debug Service):\n" 
		+ ot::ThisComputerInfo::toInfoString(ot::ThisComputerInfo::GatherAllMode) + "\n"
	);
}

void Application::testKill(void) {
	std::thread t([]() {
		auto ui = Application::instance().getUiComponent();
		ui->displayMessage("Killing debug service in 3...\n");
		std::this_thread::sleep_for(std::chrono::seconds(1));
		ui->displayMessage("Killing debug service in 2..\n");
		std::this_thread::sleep_for(std::chrono::seconds(1));
		ui->displayMessage("Killing debug service in 1.\n");
		std::this_thread::sleep_for(std::chrono::seconds(1));
		ui->displayMessage("Killing debug service now!\n");
		exit(0);
		}
	);
	t.detach();
}

void Application::testTableSmall(void) {
	std::thread t(&Application::sendTableWorker, this, 1000, 100);
	t.detach();
}

void Application::testTableMedium(void) {
	std::thread t(&Application::sendTableWorker, this, 10000, 100);
	t.detach();
}

void Application::testTableBig(void) {
	std::thread t(&Application::sendTableWorker, this, 10000, 1000);
	t.detach();
}

void Application::createPlotOneCurve()
{
	const std::string collName = getCollectionName();
	ResultCollectionExtender extender(collName, *getModelComponent(), &getClassFactory(), OT_INFO_SERVICE_TYPE_ImportParameterizedDataService);
	PlotBuilder builder(extender);

	//Single curve
	DatasetDescription description;
	MetadataParameter parameter;
	parameter.parameterName = "Frequency";
	parameter.typeName = ot::TypeNames::getFloatTypeName();
	parameter.unit = "kHz";
	std::unique_ptr<QuantityDescriptionCurve> quantDesc(new QuantityDescriptionCurve());
	quantDesc->setName("Magnitude");
	quantDesc->addValueDescription("", ot::TypeNames::getInt32TypeName(), "dB");

	for (float i = 0.; i <= 50.; i++)
	{
		quantDesc->addDatapoint(ot::Variable(static_cast<int32_t>(i)));
		parameter.values.push_back(ot::Variable(i));
	}
	std::shared_ptr<ParameterDescription> parameterDesc(new ParameterDescription(parameter, false));

	description.setQuantityDescription(quantDesc.release());
	description.addParameterDescription(parameterDesc);

	const std::string plotName = "Test/A_plot_Single";
	ot::Plot1DCurveCfg curveCfg = EntityResult1DCurve::createDefaultConfig(plotName, "A_Curve");
	builder.addCurve(std::move(description), curveCfg, "SingleCurve");

	//Here the shared part
	ot::Plot1DCfg plotCfg;
	plotCfg.setEntityName(plotName);
	builder.buildPlot(plotCfg);
}

void Application::createPlotTwoCurves() {
	const std::string collName = getCollectionName();
	ResultCollectionExtender extender(collName, *getModelComponent(), &getClassFactory(), OT_INFO_SERVICE_TYPE_ImportParameterizedDataService);
	PlotBuilder builder(extender);

	// First curve
	DatasetDescription description;
	MetadataParameter parameter;
	parameter.parameterName = "Frequency";
	parameter.typeName = ot::TypeNames::getFloatTypeName();
	parameter.unit = "kHz";
	std::unique_ptr<QuantityDescriptionCurve> quantDesc(new QuantityDescriptionCurve());
	quantDesc->setName("Magnitude");
	quantDesc->addValueDescription("", ot::TypeNames::getInt32TypeName(), "dB");

	for (float i = 0.; i <= 50.; i++) {
		quantDesc->addDatapoint(ot::Variable(static_cast<int32_t>(i)));
		parameter.values.push_back(ot::Variable(i));
	}
	std::shared_ptr<ParameterDescription> parameterDesc(new ParameterDescription(parameter, false));

	description.setQuantityDescription(quantDesc.release());
	description.addParameterDescription(parameterDesc);

	ot::Plot1DCurveCfg curveCfg;
	curveCfg.setLinePenColor(ot::Color(ot::DefaultColor::Blue));
	const std::string plotName = "Test/A_plot_Double";
	curveCfg.setEntityName(plotName + "/A_Curve1");

	builder.addCurve(std::move(description), curveCfg, "TwoCurves1");

	// Second curve
	DatasetDescription description2;
	MetadataParameter parameter2;
	parameter2.parameterName = "Frequency";
	parameter2.typeName = ot::TypeNames::getFloatTypeName();
	parameter2.unit = "kHz";
	std::unique_ptr<QuantityDescriptionCurve> quantDesc2(new QuantityDescriptionCurve());
	quantDesc2->setName("Magnitude");
	quantDesc2->addValueDescription("", ot::TypeNames::getInt32TypeName(), "dB");

	for (float i = 0.; i <= 50.; i++) {
		quantDesc2->addDatapoint(ot::Variable(-static_cast<int32_t>(i)));
		parameter2.values.push_back(ot::Variable(i));
	}
	std::shared_ptr<ParameterDescription> parameterDesc2(new ParameterDescription(parameter2, false));

	description2.setQuantityDescription(quantDesc2.release());
	description2.addParameterDescription(parameterDesc2);

	ot::Plot1DCurveCfg curveCfg2;
	curveCfg2.setLinePenColor(ot::Color(ot::DefaultColor::Red));
	curveCfg2.setEntityName(plotName + "/A_Curve2");

	builder.addCurve(std::move(description2), curveCfg2, "TwoCurves2");

	// Here the shared part
	ot::Plot1DCfg plotCfg;
	plotCfg.setEntityName(plotName);
	builder.buildPlot(plotCfg);
}

void Application::createFamilyOfCurves()
{
	const std::string collName = getCollectionName();
	ResultCollectionExtender extender(collName, *getModelComponent(), &getClassFactory(), OT_INFO_SERVICE_TYPE_ImportParameterizedDataService);
	PlotBuilder builder(extender);

	MetadataParameter parameter;
	parameter.parameterName = "Frequency";
	parameter.typeName = ot::TypeNames::getFloatTypeName();
	parameter.unit = "kHz";

	const std::string plotName = "Test/A_plot_2Param";

	std::vector<float> offsets{ 3.5f,7.2f,13.f };
	std::shared_ptr<ParameterDescription> parameterDesc = nullptr;
	std::list<DatasetDescription> descriptions;
	for (int runID = 0; runID < 3; runID++)
	{
		DatasetDescription description;

		std::unique_ptr<QuantityDescriptionCurve> quantDesc(new QuantityDescriptionCurve());
		quantDesc->setName("S_11 (Magnitude)");
		quantDesc->addValueDescription("", ot::TypeNames::getFloatTypeName(), "dB");

		for (float i = 0.; i <= 50.; i++)
		{
			float value = i * (runID + 1);
			quantDesc->addDatapoint(ot::Variable(value));
			parameter.values.push_back(ot::Variable(i));
		}
		if (parameterDesc == nullptr)
		{
			parameterDesc.reset(new ParameterDescription(parameter, false));
		}

		MetadataParameter additionalParameter;
		additionalParameter.parameterName = "Offset";
		additionalParameter.values.push_back(offsets[runID]);
		additionalParameter.typeName = ot::TypeNames::getFloatTypeName();
		additionalParameter.unit = "mm";
		std::shared_ptr<ParameterDescription> additionalParameterDescription(new ParameterDescription(additionalParameter, true));

		description.setQuantityDescription(quantDesc.release());
		description.addParameterDescription(parameterDesc);
		description.addParameterDescription(additionalParameterDescription);
		descriptions.push_back(std::move(description));
	}

	ot::PainterRainbowIterator rainbowPainterIt;
	
	ot::Plot1DCurveCfg curveCfg;
	curveCfg.setEntityName(plotName + "/A_Curve");
	auto stylePainter = rainbowPainterIt.getNextPainter();

	curveCfg.setLinePenPainter(stylePainter.release());
	builder.addCurve(std::move(descriptions), curveCfg, "FamilyOfCurve");

	//Here the shared part
	ot::Plot1DCfg plotCfg;
	plotCfg.setEntityName(plotName);
	builder.buildPlot(plotCfg);
}

void Application::createFamilyOfCurves3ParameterConst()
{
	const std::string collName = getCollectionName();

	ResultCollectionExtender extender(collName, *getModelComponent(), &getClassFactory(), OT_INFO_SERVICE_TYPE_ImportParameterizedDataService);
	PlotBuilder builder(extender);

	MetadataParameter parameter;
	parameter.parameterName = "Frequency";
	parameter.typeName = ot::TypeNames::getFloatTypeName();
	parameter.unit = "kHz";



	ot::Plot1DCurveCfg curveCfg;
	curveCfg.setLinePenColor(ot::Color(ot::DefaultColor::Blue));
	const std::string plotName = "Test/A_plot_3Param_const";
	curveCfg.setEntityName(plotName + "/A_FamilyOfCurves");

	std::vector<float> offsets{ 3.5f,7.2f,13.f };
	std::shared_ptr<ParameterDescription> parameterDesc = nullptr;
	std::list<DatasetDescription> descriptions;
	for (int material = 0; material < 3; material++)
	{
		for (int runID = 0; runID < 1; runID++)
		{
			DatasetDescription description;

			std::unique_ptr<QuantityDescriptionCurve> quantDesc(new QuantityDescriptionCurve());
			quantDesc->setName("S_11 (Magnitude)");
			quantDesc->addValueDescription("", ot::TypeNames::getFloatTypeName(), "dB");

			for (float i = 0.; i <= 50.; i++)
			{
				float value = static_cast<float>((i * (runID + 1))) * powf(-1,static_cast<float>(material));
				quantDesc->addDatapoint(ot::Variable(value));
				parameter.values.push_back(ot::Variable(i));
			}
			if (parameterDesc == nullptr)
			{
				parameterDesc.reset(new ParameterDescription(parameter, false));
			}

			MetadataParameter additionalParameter;
			additionalParameter.parameterName = "Offset";
			additionalParameter.values.push_back(offsets[runID]);
			additionalParameter.typeName = ot::TypeNames::getFloatTypeName();
			additionalParameter.unit = "mm";
			std::shared_ptr<ParameterDescription> additionalParameterDescription(new ParameterDescription(additionalParameter, true));

			MetadataParameter additionalParameter2;
			additionalParameter2.parameterName = "Radius";
			additionalParameter2.values.push_back(material);
			additionalParameter2.typeName = ot::TypeNames::getInt32TypeName();
			additionalParameter2.unit = "mm";
			std::shared_ptr<ParameterDescription> additionalParameterDescription2(new ParameterDescription(additionalParameter2, true));

			description.setQuantityDescription(quantDesc.release());
			description.addParameterDescription(parameterDesc);
			description.addParameterDescription(additionalParameterDescription);
			description.addParameterDescription(additionalParameterDescription2);
			descriptions.push_back(std::move(description));
		}
	}
	builder.addCurve(std::move(descriptions), curveCfg, "FamilyOfCurve_3PConst");


	//Here the shared part
	ot::Plot1DCfg plotCfg;
	plotCfg.setEntityName(plotName);
	builder.buildPlot(plotCfg);
}

void Application::createFamilyOfCurves3Parameter()
{
	const std::string collName = getCollectionName();

	ResultCollectionExtender extender(collName, *getModelComponent(), &getClassFactory(), OT_INFO_SERVICE_TYPE_ImportParameterizedDataService);
	PlotBuilder builder(extender);

	MetadataParameter parameter;
	parameter.parameterName = "Frequency";
	parameter.typeName = ot::TypeNames::getFloatTypeName();
	parameter.unit = "kHz";



	ot::Plot1DCurveCfg curveCfg;
	curveCfg.setLinePenColor(ot::Color(ot::DefaultColor::Blue));
	const std::string plotName = "Test/A_plot_3Param";
	curveCfg.setEntityName(plotName + "/A_FamilyOfCurves");

	std::vector<float> offsets{ 3.5f,7.2f,13.f };
	std::shared_ptr<ParameterDescription> parameterDesc = nullptr;
	std::list<DatasetDescription> descriptions;
	for (int material = 0; material < 3; material++)
	{
		for (int runID = 0; runID < 3; runID++)
		{
			DatasetDescription description;

			std::unique_ptr<QuantityDescriptionCurve> quantDesc(new QuantityDescriptionCurve());
			quantDesc->setName("S_11 (Magnitude)");
			quantDesc->addValueDescription("", ot::TypeNames::getFloatTypeName(), "dB");

			for (float i = 0.; i <= 50.; i++)
			{
				float value = static_cast<float>((i * (runID + 1))) * powf(-1, static_cast<float>(material));
				quantDesc->addDatapoint(ot::Variable(value));
				parameter.values.push_back(ot::Variable(i));
			}
			if (parameterDesc == nullptr)
			{
				parameterDesc.reset(new ParameterDescription(parameter, false));
			}

			MetadataParameter additionalParameter;
			additionalParameter.parameterName = "Offset";
			additionalParameter.values.push_back(offsets[runID]);
			additionalParameter.typeName = ot::TypeNames::getFloatTypeName();
			additionalParameter.unit = "mm";
			std::shared_ptr<ParameterDescription> additionalParameterDescription(new ParameterDescription(additionalParameter, true));

			MetadataParameter additionalParameter2;
			additionalParameter2.parameterName = "Radius";
			additionalParameter2.values.push_back(material);
			additionalParameter2.typeName = ot::TypeNames::getInt32TypeName();
			additionalParameter2.unit = "mm";
			std::shared_ptr<ParameterDescription> additionalParameterDescription2(new ParameterDescription(additionalParameter2, true));

			description.setQuantityDescription(quantDesc.release());
			description.addParameterDescription(parameterDesc);
			description.addParameterDescription(additionalParameterDescription);
			description.addParameterDescription(additionalParameterDescription2);
			descriptions.push_back(std::move(description));
		}
	}
	builder.addCurve(std::move(descriptions), curveCfg, "FamilyOfCurve_3P");


	//Here the shared part
	ot::Plot1DCfg plotCfg;
	plotCfg.setEntityName(plotName);
	builder.buildPlot(plotCfg);
}

void Application::createPlotScatter() {
	const std::string collName = getCollectionName();
	ResultCollectionExtender extender(collName, *getModelComponent(), &getClassFactory(), OT_INFO_SERVICE_TYPE_ImportParameterizedDataService);
	PlotBuilder builder(extender);

	//Single curve
	DatasetDescription description;
	MetadataParameter parameter;
	parameter.parameterName = "Frequency";
	parameter.typeName = ot::TypeNames::getFloatTypeName();
	parameter.unit = "kHz";
	std::unique_ptr<QuantityDescriptionCurve> quantDesc(new QuantityDescriptionCurve());
	quantDesc->setName("Magnitude");
	quantDesc->addValueDescription("", ot::TypeNames::getInt32TypeName(), "dB");

	for (float i = 0.; i <= 50.; i++) {
		quantDesc->addDatapoint(ot::Variable(rand() % 100));
		parameter.values.push_back(ot::Variable(i));
	}
	std::shared_ptr<ParameterDescription> parameterDesc(new ParameterDescription(parameter, false));

	description.setQuantityDescription(quantDesc.release());
	description.addParameterDescription(parameterDesc);

	const std::string plotName = "Test/A_plot_Scatter";
	ot::Plot1DCurveCfg curveCfg = EntityResult1DCurve::createDefaultConfig(plotName, "ScatterCurve", EntityResult1DCurve::ScatterPlot);
	builder.addCurve(std::move(description), curveCfg, "Scatter");

	//Here the shared part
	ot::Plot1DCfg plotCfg;
	plotCfg.setEntityName(plotName);
	builder.buildPlot(plotCfg);
}

void Application::createPlotSinglePoint() {
	const std::string collName = getCollectionName();
	ResultCollectionExtender extender(collName, *getModelComponent(), &getClassFactory(), OT_INFO_SERVICE_TYPE_ImportParameterizedDataService);
	PlotBuilder builder(extender);

	// First curve
	DatasetDescription description;
	MetadataParameter parameter;
	parameter.parameterName = "Frequency";
	parameter.typeName = ot::TypeNames::getFloatTypeName();
	parameter.unit = "kHz";
	std::unique_ptr<QuantityDescriptionCurve> quantDesc(new QuantityDescriptionCurve());
	quantDesc->setName("Magnitude");
	quantDesc->addValueDescription("", ot::TypeNames::getInt32TypeName(), "dB");

	for (float i = 0.; i < 1; i++) {
		quantDesc->addDatapoint(ot::Variable(static_cast<int32_t>(i)));
		parameter.values.push_back(ot::Variable(i));
	}
	std::shared_ptr<ParameterDescription> parameterDesc(new ParameterDescription(parameter, false));

	description.setQuantityDescription(quantDesc.release());
	description.addParameterDescription(parameterDesc);

	ot::Plot1DCurveCfg curveCfg;
	curveCfg.setLinePenColor(ot::Color(ot::DefaultColor::Blue));
	const std::string plotName = "Test/A_plot_SingleValue";
	curveCfg.setEntityName(plotName + "/SingleValueCurve1");

	builder.addCurve(std::move(description), curveCfg, "SingleValueCurve1");

	// Second curve
	DatasetDescription description2;
	MetadataParameter parameter2;
	parameter2.parameterName = "Frequency";
	parameter2.typeName = ot::TypeNames::getFloatTypeName();
	parameter2.unit = "kHz";
	std::unique_ptr<QuantityDescriptionCurve> quantDesc2(new QuantityDescriptionCurve());
	quantDesc2->setName("Magnitude");
	quantDesc2->addValueDescription("", ot::TypeNames::getInt32TypeName(), "dB");

	for (float i = 0.; i < 1; i++) {
		quantDesc2->addDatapoint(ot::Variable(-static_cast<int32_t>(i)));
		parameter2.values.push_back(ot::Variable(i));
	}
	std::shared_ptr<ParameterDescription> parameterDesc2(new ParameterDescription(parameter2, false));

	description2.setQuantityDescription(quantDesc2.release());
	description2.addParameterDescription(parameterDesc2);

	ot::Plot1DCurveCfg curveCfg2;
	curveCfg2.setLinePenColor(ot::Color(ot::DefaultColor::Red));
	curveCfg2.setEntityName(plotName + "/SingleValueCurve2");

	builder.addCurve(std::move(description2), curveCfg2, "SingleValueCurve2");

	// Here the shared part
	ot::Plot1DCfg plotCfg;
	plotCfg.setEntityName(plotName);
	builder.buildPlot(plotCfg);
}

void Application::sendTableWorker(int _rows, int _columns) {
	using namespace ot;

	auto ui = this->getUiComponent();
	if (!ui) {
		OT_LOG_E("No ui? How?");
		return;
	}

	// Check interval
	RuntimeIntervalTest intervalTest;

	// Display start info
	StyledTextBuilder startInfo;
	startInfo << "Test table with " + std::to_string(_rows) + " rows and " + std::to_string(_columns) + " columns. Started at " << StyledText::TimeHHMMSSZZZZ << " (user computer time).";
	ui->displayStyledMessage(startInfo);

	// Create table document
	JsonDocument doc;
	this->getBasicServiceInformation().addToJsonObject(doc, doc.GetAllocator());

	doc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_TABLE_Setup, doc.GetAllocator()), doc.GetAllocator());

	// Fill cells with some default values
	TableCfg cfg(_rows, _columns);
	for (int r = 0; r < _rows; r++) {
		for (int c = 0; c < _columns; c++) {
			cfg.setCellText(r, c, "Cell " + std::to_string(r) + ":" + std::to_string(c));
		}
	}
	cfg.setEntityName("Test (" + std::to_string(m_nameCounter++) + "): Table");
	cfg.setCellText(0, 0, "Cell count: " + std::to_string(_rows * _columns));

	JsonObject cfgObj;
	cfg.addToJsonObject(cfgObj, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Config, cfgObj, doc.GetAllocator());

	std::string resp;
	ui->sendMessage(true, doc, resp);

	// Display end info
	StyledTextBuilder endInfo;
	endInfo << "Table test finished at " << StyledText::TimeHHMMSSZZZZ << " (user computer time). Debug service needed " + intervalTest.currentIntervalString() + " to handle the button event.";
	ui->displayStyledMessage(endInfo);
}

// ###########################################################################################################################################################################################################################################################################################################################

// General feature handling

void Application::actionAboutToBePerformed(const char* _json) {
	ot::JsonDocument doc;
	if (!doc.fromJson(_json)) {
		OT_LOG_EAS("Failed to deserialize request: \"" + std::string(_json) + "\"");
		return;
	}

	std::string action = ot::json::getString(doc, OT_ACTION_MEMBER);

	if (action == OT_ACTION_CMD_Ping) {
		if (this->getFeatureEnabled(DebugServiceConfig::FeatureFlag::ExitOnPing)) {
			OT_LOG_T("Performing exit on ping");
			exit(0);
		}
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// Default methods

Application& Application::instance() {
	static Application g_instance;
	return g_instance;
}

Application::~Application()
{

}

/*
std::string Application::handleExecuteModelAction(ot::JsonDocument& _document) {
	std::string action = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ActionName);

	ButtonInfo* info = nullptr;
	for (ButtonInfo& btn : m_testButtons) {
		if (action == this->getButtonKey(btn)) {
			btn.callback();
			return std::string();
		}
	}
	
	OT_LOG_WAS("Unknown model action \"" + action + "\"");
	return std::string();
}*/

// ##################################################################################################################################################################################################################

std::string Application::getButtonKey(const ButtonInfo& _info) const {
	return OT_DEBUG_SERVICE_PAGE_NAME ":" + _info.group + ":" + _info.name;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Required functions

void Application::initialize() {
	if (this->getFeatureEnabled(DebugServiceConfig::ExitOnInit)) {
		OT_LOG_T("Performing exit on init");
		exit(0);
	}
}

void Application::run() {
	if (this->getFeatureEnabled(DebugServiceConfig::FeatureFlag::ExitOnRun)) {
		OT_LOG_T("Performing exit on run");
		exit(0);
	}
}

void Application::uiConnected(ot::components::UiComponent * _ui)
{
	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, true);

	_ui->addMenuPage(OT_DEBUG_SERVICE_PAGE_NAME);

	std::list<std::string> addedGroups;
	for (const auto& it : m_testButtons) {
		// Add group if needed
		if (std::find(addedGroups.begin(), addedGroups.end(), it.group) == addedGroups.end()) {
			_ui->addMenuGroup(OT_DEBUG_SERVICE_PAGE_NAME, it.group);
			addedGroups.push_back(it.group);
		}

		_ui->addMenuButton(OT_DEBUG_SERVICE_PAGE_NAME, it.group, it.name, it.title, ot::LockModelWrite | ot::LockViewRead | ot::LockViewWrite, it.icon);
		this->connectButton(it.callback, OT_DEBUG_SERVICE_PAGE_NAME ":" + it.group + ":" + it.name);
	}

	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, false);
}

void Application::preShutdown(void) {
	if (this->getFeatureEnabled(DebugServiceConfig::FeatureFlag::ExitOnPreShutdown)) {
		OT_LOG_T("Performing exit on pre shutdown");
		exit(0);
	}
}
