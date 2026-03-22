// @otlicense
// File: EntityResult1DCurve.cpp
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

// OpenTwin header
#include "OTCore/EntityName.h"
#include "OTGui/VisualisationTypes.h"
#include "OTGui/Dialog/Painter2DDialogFilterDefaults.h"
#include "OTGui/Painter/Painter2D.h"
#include "OTGui/Painter/StyleRefPainter2D.h"
#include "OTGui/Style/ColorStyleTypes.h"
#include "OTCommunication/ActionTypes.h"
#include "OTModelEntities/EntityResult1DCurve.h"
#include "OTModelEntities/PropertyHelper.h"

static EntityFactoryRegistrar<EntityResult1DCurve> registrar("EntityResult1DCurve");

ot::Plot1DCurveCfg EntityResult1DCurve::createDefaultConfig(const std::string& _plotName, const std::string& _curveName, DefaultCurveStyle _style)
{
	ot::Plot1DCurveCfg cfg;

	cfg.setEntityName(_plotName + "/" + _curveName);
	cfg.setPointColorFromCurve(true);

	switch (_style)
	{
	case DefaultCurveStyle::Default:
		cfg.setLinePenStyle(ot::LineStyle::SolidLine);
		cfg.setLinePenColor(ot::ColorStyleValueEntry::PlotCurve);
		cfg.setLinePenWidth(2.);
		cfg.setPointSymbol(ot::Plot1DCurveCfg::NoSymbol);
		cfg.setPointSize(5);
		cfg.setPointInterval(1);
		cfg.setPointOutlinePenColor(ot::ColorStyleValueEntry::PlotCurveSymbol);
		cfg.setPointOutlinePenWidth(1.);
		cfg.setPointFillColor(ot::ColorStyleValueEntry::PlotCurveSymbol);
		break;

	case DefaultCurveStyle::ScatterPlot:
		cfg.setLinePenStyle(ot::LineStyle::NoLine);
		cfg.setLinePenColor(ot::ColorStyleValueEntry::PlotCurve);
		cfg.setLinePenWidth(2.);
		cfg.setPointSymbol(ot::Plot1DCurveCfg::Circle);
		cfg.setPointSize(10);
		cfg.setPointInterval(1);
		cfg.setPointOutlinePenColor(ot::ColorStyleValueEntry::PlotCurveSymbol);
		cfg.setPointOutlinePenWidth(1.);
		cfg.setPointFillColor(ot::ColorStyleValueEntry::PlotCurveSymbol);
		break;

	default:
		OT_LOG_EAS("Unknown default curve style (" + std::to_string(static_cast<int>(_style)) + ")");
		break;
	}

	return cfg;
}

EntityResult1DCurve::EntityResult1DCurve(ot::UID _ID, EntityBase* _parent, EntityObserver* _mdl, ModelState* _ms)
	: EntityBase(_ID, _parent, _mdl, _ms)
{
	ot::EntityTreeItem treeItem = getTreeItem();
	treeItem.setVisibleIcon("Default/Plot1DVisible");
	treeItem.setHiddenIcon("Default/Plot1DHidden");
	this->setDefaultTreeItem(treeItem);

	ot::VisualisationTypes visTypes = getVisualizationTypes();
	visTypes.addCurveVisualisation();
	this->setDefaultVisualizationTypes(visTypes);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Virtual methods

void EntityResult1DCurve::addVisualizationNodes()
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_AddSceneNode, doc.GetAllocator()), doc.GetAllocator());

	doc.AddMember(OT_ACTION_PARAM_TreeItem, ot::JsonObject(this->getTreeItem(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_VisualizationTypes, ot::JsonObject(this->getVisualizationTypes(), doc.GetAllocator()), doc.GetAllocator());

	getObserver()->sendMessageToViewer(doc);
}

bool EntityResult1DCurve::updateFromProperties()
{
	// Update the curve displayed in the frontend after the property change
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UpdateCurvesOfPlot, doc.GetAllocator()), doc.GetAllocator());

	const std::string plotName = getParent()->getName();
	doc.AddMember(OT_ACTION_PARAM_NAME, ot::JsonString(plotName, doc.GetAllocator()), doc.GetAllocator());

	ot::JsonObject curveCfgSerialised;
	ot::Plot1DCurveCfg curveCfg = getCurve();
	curveCfg.addToJsonObject(curveCfgSerialised, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_VIEW1D_CurveConfigs, curveCfgSerialised, doc.GetAllocator());

	ot::VisualisationCfg visualisationCfg;
	visualisationCfg.setVisualisationType(OT_ACTION_CMD_VIEW1D_Setup);
	visualisationCfg.setOverrideViewerContent(false);
	visualisationCfg.setAsActiveView(true);

	doc.AddMember(OT_ACTION_PARAM_VisualisationConfig, ot::JsonObject(visualisationCfg, doc.GetAllocator()), doc.GetAllocator());
	getObserver()->sendMessageToViewer(doc);

	return this->updatePropertyVisibilities();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Configuration

void EntityResult1DCurve::createProperties(DefaultCurveStyle _style)
{
	// Prepare default values depending on set style
	std::string defaultLineStyle;
	std::string defaultSymbol;
	int defaultSymbolSize = 5;
	bool useCurveColorForSymbol = true;

	switch (_style)
	{
	case EntityResult1DCurve::Default:
		defaultLineStyle = ot::toString(ot::LineStyle::SolidLine);
		defaultSymbol = ot::Plot1DCurveCfg::toString(ot::Plot1DCurveCfg::NoSymbol);
		defaultSymbolSize = 5;
		useCurveColorForSymbol = true;
		break;

	case EntityResult1DCurve::ScatterPlot:
		defaultLineStyle = ot::toString(ot::LineStyle::NoLine);
		defaultSymbol = ot::Plot1DCurveCfg::toString(ot::Plot1DCurveCfg::Circle);
		defaultSymbolSize = 10;
		useCurveColorForSymbol = false;
		break;

	default:
		OT_LOG_EAS("Unknown default curve style (" + std::to_string(static_cast<int>(_style)) + ")");
		defaultLineStyle = ot::toString(ot::LineStyle::SolidLine);
		defaultSymbol = ot::Plot1DCurveCfg::toString(ot::Plot1DCurveCfg::NoSymbol);
		defaultSymbolSize = 5;
		break;
	}

	// Line color
	EntityPropertiesGuiPainter* colorProp = EntityPropertiesGuiPainter::createProperty("General", "Color", new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::PlotCurve), "", getProperties());
	colorProp->setToolTip("The color of the curve line in the plot.");
	colorProp->setFilter(ot::Painter2DDialogFilterDefaults::plotCurve());

	// Line width
	EntityPropertiesInteger* widthProp = EntityPropertiesInteger::createProperty("General", "Line Width", 2, 1, 99, "", getProperties());
	widthProp->setToolTip("The width of the curve line in the plot.");
	widthProp->setAllowCustomValues(false);

	// Line style
	EntityPropertiesSelection* selectionProp = EntityPropertiesSelection::createProperty("General", "Line Style", {
		ot::toString(ot::LineStyle::NoLine),
		ot::toString(ot::LineStyle::SolidLine),
		ot::toString(ot::LineStyle::DashLine),
		ot::toString(ot::LineStyle::DotLine),
		ot::toString(ot::LineStyle::DashDotLine),
		ot::toString(ot::LineStyle::DashDotDotLine)
		},
		defaultLineStyle, "", getProperties());
	selectionProp->setToolTip("The style of the curve line in the plot.");

	// Symbol style
	EntityPropertiesSelection* symbolProp = EntityPropertiesSelection::createProperty("General", "Symbol", {
		ot::Plot1DCurveCfg::toString(ot::Plot1DCurveCfg::NoSymbol),
		ot::Plot1DCurveCfg::toString(ot::Plot1DCurveCfg::Circle),
		ot::Plot1DCurveCfg::toString(ot::Plot1DCurveCfg::Square),
		ot::Plot1DCurveCfg::toString(ot::Plot1DCurveCfg::Diamond),
		ot::Plot1DCurveCfg::toString(ot::Plot1DCurveCfg::TriangleUp),
		ot::Plot1DCurveCfg::toString(ot::Plot1DCurveCfg::TriangleDown),
		ot::Plot1DCurveCfg::toString(ot::Plot1DCurveCfg::TriangleLeft),
		ot::Plot1DCurveCfg::toString(ot::Plot1DCurveCfg::TriangleRight),
		ot::Plot1DCurveCfg::toString(ot::Plot1DCurveCfg::Cross),
		ot::Plot1DCurveCfg::toString(ot::Plot1DCurveCfg::XCross),
		ot::Plot1DCurveCfg::toString(ot::Plot1DCurveCfg::HLine),
		ot::Plot1DCurveCfg::toString(ot::Plot1DCurveCfg::VLine),
		ot::Plot1DCurveCfg::toString(ot::Plot1DCurveCfg::Star6),
		ot::Plot1DCurveCfg::toString(ot::Plot1DCurveCfg::Star8),
		ot::Plot1DCurveCfg::toString(ot::Plot1DCurveCfg::Hexagon)
		},
		defaultSymbol, "", getProperties());
	symbolProp->setToolTip("The symbol that is used for the curve data points in the plot.");

	// Symbol Size
	EntityPropertiesInteger* symbolSizeProp = EntityPropertiesInteger::createProperty("General", "Symbol Size", defaultSymbolSize, 1, 99, "", getProperties());
	symbolSizeProp->setToolTip("The size of the curve data point symbols in the plot.");
	symbolSizeProp->setAllowCustomValues(false);

	// Symbol Interval
	EntityPropertiesInteger* symbolIntervalProp = EntityPropertiesInteger::createProperty("General", "Symbol Interval", 1, 1, 99, "", getProperties());
	symbolIntervalProp->setToolTip("The interval of the curve data point symbols in the plot. A value of 1 means that every data point is shown, a value of 2 means that every second data point is shown, etc.");
	symbolIntervalProp->setAllowCustomValues(false);

	// Symbol Color from Curve Color
	EntityPropertiesBoolean* symbolUseCurveColorProp = EntityPropertiesBoolean::createProperty("General", "Symbol use Curve Color", useCurveColorForSymbol, "", getProperties());

	// Symbol Outline Color
	EntityPropertiesGuiPainter* symbolOutlineColorProp = EntityPropertiesGuiPainter::createProperty("General", "Symbol Outline Color", new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::PlotCurveSymbol), "", getProperties());
	symbolOutlineColorProp->setToolTip("The color of the curve data point symbols outline in the plot.");
	symbolOutlineColorProp->setFilter(ot::Painter2DDialogFilterDefaults::plotCurve());

	// Symbol Outline Width
	EntityPropertiesInteger* symbolOutlineWidthProp = EntityPropertiesInteger::createProperty("General", "Symbol Outline Width", 1, 1, 99, "", getProperties());
	symbolOutlineWidthProp->setToolTip("The width of the curve data point symbols outline in the plot.");
	symbolOutlineWidthProp->setAllowCustomValues(false);

	// Symbol Fill Color
	EntityPropertiesGuiPainter* symbolFillColorProp = EntityPropertiesGuiPainter::createProperty("General", "Symbol Fill Color", new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::PlotCurveSymbol), "", getProperties());
	symbolFillColorProp->setToolTip("The fill color of the curve data point symbols in the plot.");
	symbolFillColorProp->setFilter(ot::Painter2DDialogFilterDefaults::plotCurve(true));
	
	m_queryProperties.setProperties(this);

	this->updatePropertyVisibilities();

	getProperties().forceResetUpdateForAllProperties();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

ot::Plot1DCurveCfg EntityResult1DCurve::getCurve()
{
	ot::Plot1DCurveCfg curveCfg;

	const std::string entityName = getName();
	curveCfg.setEntityName(entityName);
	curveCfg.setEntityID(getEntityID());
	curveCfg.setEntityVersion(getEntityStorageVersion());

	const ot::Painter2D* painter = PropertyHelper::getPainterPropertyValue(this, "Color");
	ot::PenFCfg penCfg(painter->createCopy());

	try
	{
		penCfg.setWidth(static_cast<double>(PropertyHelper::getIntegerPropertyValue(this, "Line Width")));
		penCfg.setStyle(ot::stringToLineStyle(PropertyHelper::getSelectionPropertyValue(this, "Line Style")));

		curveCfg.setPointSymbol(ot::Plot1DCurveCfg::stringToSymbol(PropertyHelper::getSelectionPropertyValue(this, "Symbol")));
		curveCfg.setPointSize(PropertyHelper::getIntegerPropertyValue(this, "Symbol Size"));
		curveCfg.setPointInterval(PropertyHelper::getIntegerPropertyValue(this, "Symbol Interval"));

		const ot::Painter2D* symbolOutlinePainter = PropertyHelper::getPainterPropertyValue(this, "Symbol Outline Color");
		ot::PenFCfg symbolPenCfg(symbolOutlinePainter->createCopy());
		symbolPenCfg.setWidth(PropertyHelper::getIntegerPropertyValue(this, "Symbol Outline Width"));
		curveCfg.setPointOutlinePen(symbolPenCfg);

		const ot::Painter2D* symbolFillPainter = PropertyHelper::getPainterPropertyValue(this, "Symbol Fill Color");
		curveCfg.setPointFillPainter(symbolFillPainter->createCopy());
		curveCfg.setPointColorFromCurve(PropertyHelper::getBoolPropertyValue(this, "Symbol use Curve Color"));
	}
	catch (const std::exception& _e)
	{
		OT_LOG_T("Legacy curve: " + std::string(_e.what()));
		// Legacy
		penCfg.setWidth(2);
		penCfg.setStyle(ot::LineStyle::SolidLine);

		curveCfg.setPointSymbol(ot::Plot1DCurveCfg::NoSymbol);
		curveCfg.setPointColorFromCurve(true);
	}

	curveCfg.setLinePen(penCfg);

	std::string curveLabel("");

	curveLabel = getName();
	auto shortName = ot::EntityName::getSubName(curveLabel);
	if (shortName.has_value())
	{
		curveLabel = shortName.value();
	}
	else
	{
		assert(false); //Failed to get the short name which should always be possible
		curveLabel = "";
	}


	curveCfg.setTitle(curveLabel);
	curveCfg.setQueryInformation(m_queryInformation);

	return curveCfg;
}

void EntityResult1DCurve::setCurve(const ot::Plot1DCurveCfg& _curve)
{
	PropertyHelper::setPainterPropertyValue(_curve.getLinePen().getPainter(), this, "Color");

	try
	{
		EntityPropertiesInteger* widthProp = PropertyHelper::getIntegerProperty(this, "Line Width");
		if (widthProp)
		{
			widthProp->setValue(static_cast<long>(_curve.getLinePen().getWidth()));
		}

		EntityPropertiesSelection* styleProp = PropertyHelper::getSelectionProperty(this, "Line Style");
		if (styleProp)
		{
			styleProp->setValue(ot::toString(_curve.getLinePen().getStyle()));
		}

		EntityPropertiesSelection* symbolProp = PropertyHelper::getSelectionProperty(this, "Symbol");
		if (symbolProp)
		{
			symbolProp->setValue(ot::Plot1DCurveCfg::toString(_curve.getPointSymbol()));
		}

		EntityPropertiesInteger* symbolSizeProp = PropertyHelper::getIntegerProperty(this, "Symbol Size");
		if (symbolSizeProp)
		{
			symbolSizeProp->setValue(static_cast<long>(_curve.getPointSize()));
		}

		EntityPropertiesInteger* symbolIntervalProp = PropertyHelper::getIntegerProperty(this, "Symbol Interval");
		if (symbolIntervalProp)
		{
			symbolIntervalProp->setValue(static_cast<long>(_curve.getPointInterval()));
		}

		EntityPropertiesBoolean* symbolUseCurveColorProp = PropertyHelper::getBoolProperty(this, "Symbol use Curve Color");
		if (symbolUseCurveColorProp)
		{
			symbolUseCurveColorProp->setValue(_curve.getPointColorFromCurve());
		}

		EntityPropertiesGuiPainter* symbolOutlineColorProp = PropertyHelper::getPainterProperty(this, "Symbol Outline Color");
		if (symbolOutlineColorProp)
		{
			symbolOutlineColorProp->setValue(_curve.getPointOutlinePen().getPainter());
		}

		EntityPropertiesInteger* symbolOutlineWidthProp = PropertyHelper::getIntegerProperty(this, "Symbol Outline Width");
		if (symbolOutlineWidthProp)
		{
			symbolOutlineWidthProp->setValue(static_cast<long>(_curve.getPointOutlinePen().getWidth()));
		}

		EntityPropertiesGuiPainter* symbolFillColorProp = PropertyHelper::getPainterProperty(this, "Symbol Fill Color");
		if (symbolFillColorProp)
		{
			symbolFillColorProp->setValue(_curve.getPointFillPainter());
		}
	}
	catch (...)
	{
		// Legacy support
	}

	this->updatePropertyVisibilities();

	m_queryInformation = _curve.getQueryInformation();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected: Virtual methods

bool EntityResult1DCurve::updatePropertyVisibilities()
{
	bool visibilityChanged = false;

	EntityPropertiesSelection* symbolProp = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty("Symbol"));
	if (symbolProp)
	{
		auto symbolSizeProp = getProperties().getProperty("Symbol Size");
		auto symbolIntervalProp = getProperties().getProperty("Symbol Interval");
		auto symbolOutlineColorProp = getProperties().getProperty("Symbol Outline Color");
		auto symbolOutlineWidthProp = getProperties().getProperty("Symbol Outline Width");
		auto symbolFillColorProp = getProperties().getProperty("Symbol Fill Color");
		auto symbolUseCurveColorProp = dynamic_cast<EntityPropertiesBoolean*>(getProperties().getProperty("Symbol use Curve Color"));

		OTAssertNullptr(symbolSizeProp);
		OTAssertNullptr(symbolIntervalProp);
		OTAssertNullptr(symbolOutlineColorProp);
		OTAssertNullptr(symbolOutlineWidthProp);
		OTAssertNullptr(symbolFillColorProp);
		OTAssertNullptr(symbolUseCurveColorProp);

		bool symbolPropsVisible = symbolProp->getValue() != ot::Plot1DCurveCfg::toString(ot::Plot1DCurveCfg::NoSymbol);
		if (symbolSizeProp->getVisible() != symbolPropsVisible)
		{
			symbolSizeProp->setVisible(symbolPropsVisible);
			symbolIntervalProp->setVisible(symbolPropsVisible);
			symbolUseCurveColorProp->setVisible(symbolPropsVisible);

			visibilityChanged = true;
		}

		bool symbolColorVisible = symbolPropsVisible && (!symbolUseCurveColorProp->getValue());
		if (symbolOutlineColorProp->getVisible() != symbolColorVisible)
		{
			symbolOutlineColorProp->setVisible(symbolColorVisible);
			symbolOutlineWidthProp->setVisible(symbolColorVisible);
			symbolFillColorProp->setVisible(symbolColorVisible);

			visibilityChanged = true;
		}

	}
			getProperties().forceResetUpdateForAllProperties();
		}
	}
	visibilityChanged |= m_queryProperties.updatePropertyVisibility(this);
	return visibilityChanged;
}

void EntityResult1DCurve::addStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityBase::addStorageData(storage);

	const ot::QuantityContainerEntryDescription& quantityDescription = m_queryInformation.getQuantityDescription();
	bsoncxx::builder::basic::document quantityDescriptionSerialised = serialise(quantityDescription);

	bsoncxx::builder::basic::array arrayOfSubDocs;
	for (const ot::QuantityContainerEntryDescription& parameterDescr : m_queryInformation.getParameterDescriptions())
	{
		bsoncxx::builder::basic::document parameterDescrDoc = serialise(parameterDescr);
		arrayOfSubDocs.append(parameterDescrDoc.extract());
	}

	storage.append(
		bsoncxx::builder::basic::kvp("Query", m_queryInformation.getQuery()),
		bsoncxx::builder::basic::kvp("Projection", m_queryInformation.getProjection()),
		bsoncxx::builder::basic::kvp("QuantityDescription", quantityDescriptionSerialised.extract()),
		bsoncxx::builder::basic::kvp("ParameterDescription", arrayOfSubDocs.extract())
	);


}

void EntityResult1DCurve::readSpecificDataFromDataBase(const bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	m_queryInformation.setQuery(std::string(doc_view["Query"].get_string()));
	m_queryInformation.setProjection(std::string(doc_view["Projection"].get_string()));

	const auto& quantityDescriptionDoc = doc_view["QuantityDescription"].get_document();

	m_queryInformation.setQuantityDescription(deserialise(quantityDescriptionDoc));

	bsoncxx::array::view parameterDescriptions = doc_view["ParameterDescription"].get_array().value;

	for (auto parameterDescription = parameterDescriptions.begin(); parameterDescription != parameterDescriptions.end(); parameterDescription++)
	{
		const auto& parameterDoc = parameterDescription->get_document();
		ot::QuantityContainerEntryDescription parameterDesc = deserialise(parameterDoc);
		m_queryInformation.addParameterDescription(std::move(parameterDesc));
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

bsoncxx::builder::basic::document EntityResult1DCurve::serialise(const ot::QuantityContainerEntryDescription& _quantityContainerEntryDescription)
{
	bsoncxx::builder::basic::document subDocument;

	bsoncxx::builder::basic::array quantityDimensions;
	for (uint32_t dimension : _quantityContainerEntryDescription.getDimension())
	{
		quantityDimensions.append(static_cast<int32_t>(dimension));
	}
	const TupleInstance& tupleInstance = _quantityContainerEntryDescription.getTupleInstance();
	bsoncxx::builder::basic::array tupleDataTypes;
	for (const std::string& dataType : tupleInstance.getTupleElementDataTypes())
	{
		tupleDataTypes.append(dataType);
	}

	bsoncxx::builder::basic::array tupleUnits;
	for (const std::string& unit : tupleInstance.getTupleUnits())
	{
		tupleUnits.append(unit);
	}

	subDocument.append(
		bsoncxx::builder::basic::kvp("FieldName", _quantityContainerEntryDescription.getFieldName()),
		bsoncxx::builder::basic::kvp("Label", _quantityContainerEntryDescription.getLabel()),
		bsoncxx::builder::basic::kvp("DataType", tupleDataTypes),
		bsoncxx::builder::basic::kvp("Unit", tupleUnits),
		bsoncxx::builder::basic::kvp("Dimension", quantityDimensions)
	);
	return subDocument;
}

ot::QuantityContainerEntryDescription EntityResult1DCurve::deserialise(bsoncxx::v_noabi::document::view _subDocument)
{
	ot::QuantityContainerEntryDescription quantityContainerEntryDescription;
	TupleInstance tupleInstance;

	quantityContainerEntryDescription.setFieldName(std::string(_subDocument["FieldName"].get_string()));
	quantityContainerEntryDescription.setLabel(std::string(_subDocument["Label"].get_string()));

	auto dataTypes = _subDocument["DataType"].get_array();
	std::vector<std::string> tupleDataTypes;
	for (auto& element : dataTypes.value)
	{
		tupleDataTypes.push_back(element.get_string().value.data());
	}
	tupleInstance.setTupleElementDataTypes(tupleDataTypes);


	auto units = _subDocument["Unit"].get_array();
	std::vector<std::string> tupleUnits;
	for (auto& element : units.value)
	{
		tupleUnits.push_back(element.get_string().value.data());
	}
	tupleInstance.setTupleUnits(tupleUnits);

	auto quantityDimension = _subDocument["Dimension"].get_array();
	for (auto& element : quantityDimension.value)
	{
		quantityContainerEntryDescription.addDimension(static_cast<uint32_t>(element.get_int32()));
	}

	quantityContainerEntryDescription.setTupleInstance(std::move(tupleInstance));
	return quantityContainerEntryDescription;
}
