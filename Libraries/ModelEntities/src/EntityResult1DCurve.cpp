#include "EntityResult1DCurve.h"
#include "PropertyHelper.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCore/EntityName.h"
#include "OTGui/Painter2D.h"
#include "OTGui/ColorStyleTypes.h"
#include "OTGui/StyleRefPainter2D.h"
#include "OTGui/VisualisationTypes.h"
#include "OTGui/Painter2DDialogFilterDefaults.h"

static EntityFactoryRegistrar<EntityResult1DCurve> registrar("EntityResult1DCurve_New");

ot::Plot1DCurveCfg EntityResult1DCurve::createDefaultConfig(const std::string& _plotName, const std::string& _curveName, DefaultCurveStyle _style) {
	ot::Plot1DCurveCfg cfg;

	cfg.setEntityName(_plotName + "/" + _curveName);

	switch (_style) {
	case DefaultCurveStyle::Default:
		cfg.setLinePenStyle(ot::LineStyle::SolidLine);
		cfg.setLinePenColor(ot::ColorStyleValueEntry::PlotCurve);
		cfg.setLinePenWidth(1.);
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
		cfg.setLinePenWidth(1.);
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

EntityResult1DCurve::EntityResult1DCurve(ot::UID _ID, EntityBase* _parent, EntityObserver* _mdl, ModelState* _ms, const std::string& _owner)
	: EntityBase(_ID,_parent,_mdl,_ms,_owner)
{
}

// ###########################################################################################################################################################################################################################################################################################################################

// Virtual methods

void EntityResult1DCurve::addVisualizationNodes()
{
	OldTreeIcon treeIcons;
	treeIcons.size = 32;
	treeIcons.visibleIcon = "Plot1DVisible";
	treeIcons.hiddenIcon = "Plot1DHidden";

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_AddSceneNode, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_TREE_Name, ot::JsonString(this->getName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, this->getEntityID(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_IsEditable, this->getEditable(), doc.GetAllocator());

	ot::VisualisationTypes visTypes;
	visTypes.addCurveVisualisation();

	visTypes.addToJsonObject(doc, doc.GetAllocator());
	treeIcons.addToJsonDoc(doc);

	getObserver()->sendMessageToViewer(doc);
}

bool EntityResult1DCurve::updateFromProperties() {
	// Update the curve displayed in the frontend after the property change
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UpdateCurvesOfPlot, doc.GetAllocator()), doc.GetAllocator());
	
	const std::string plotName = getParent()->getName();
	doc.AddMember(OT_ACTION_PARAM_NAME, ot::JsonString(plotName, doc.GetAllocator()), doc.GetAllocator());

	ot::JsonObject curveCfgSerialised;
	ot::Plot1DCurveCfg curveCfg = getCurve();
	curveCfg.addToJsonObject(curveCfgSerialised, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_VIEW1D_CurveConfigs, curveCfgSerialised,doc.GetAllocator());

	ot::VisualisationCfg visualisationCfg;
	visualisationCfg.setVisualisationType(OT_ACTION_CMD_VIEW1D_Setup);
	visualisationCfg.setOverrideViewerContent(false);
	visualisationCfg.setAsActiveView(true);

	doc.AddMember(OT_ACTION_PARAM_Visualisation_Config, ot::JsonObject(visualisationCfg, doc.GetAllocator()), doc.GetAllocator());
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
	
	switch (_style) {
	case EntityResult1DCurve::Default:
		defaultLineStyle = ot::toString(ot::LineStyle::SolidLine);
		defaultSymbol = ot::Plot1DCurveCfg::toString(ot::Plot1DCurveCfg::NoSymbol);
		defaultSymbolSize = 5;
		break;

	case EntityResult1DCurve::ScatterPlot:
		defaultLineStyle = ot::toString(ot::LineStyle::NoLine);
		defaultSymbol = ot::Plot1DCurveCfg::toString(ot::Plot1DCurveCfg::Circle);
		defaultSymbolSize = 10;
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
	EntityPropertiesInteger* widthProp = EntityPropertiesInteger::createProperty("General", "Line Width", 1, 1, 99, "", getProperties());
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

	try {
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
	}
	catch (...) {
		// Legacy
		penCfg.setWidth(1);
		penCfg.setStyle(ot::LineStyle::SolidLine);

		curveCfg.setPointSymbol(ot::Plot1DCurveCfg::NoSymbol);
	}

	curveCfg.setLinePen(penCfg);
	
	std::string curveLabel("");
	
	curveLabel = getName();
	auto shortName = ot::EntityName::getSubName(curveLabel);
	if (shortName.has_value()) {
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

void EntityResult1DCurve::setCurve(const ot::Plot1DCurveCfg& _curve) {
	PropertyHelper::setPainterPropertyValue(_curve.getLinePen().getPainter(), this, "Color");
	
	try {
		EntityPropertiesInteger* widthProp = PropertyHelper::getIntegerProperty(this, "Line Width");
		if (widthProp) {
			widthProp->setValue(static_cast<long>(_curve.getLinePen().getWidth()));
		}

		EntityPropertiesSelection* styleProp = PropertyHelper::getSelectionProperty(this, "Line Style");
		if (styleProp) {
			styleProp->setValue(ot::toString(_curve.getLinePen().getStyle()));
		}

		EntityPropertiesSelection* symbolProp = PropertyHelper::getSelectionProperty(this, "Symbol");
		if (symbolProp) {
			symbolProp->setValue(ot::Plot1DCurveCfg::toString(_curve.getPointSymbol()));
		}

		EntityPropertiesInteger* symbolSizeProp = PropertyHelper::getIntegerProperty(this, "Symbol Size");
		if (symbolSizeProp) {
			symbolSizeProp->setValue(static_cast<long>(_curve.getPointSize()));
		}

		EntityPropertiesInteger* symbolIntervalProp = PropertyHelper::getIntegerProperty(this, "Symbol Interval");
		if (symbolIntervalProp) {
			symbolIntervalProp->setValue(static_cast<long>(_curve.getPointInterval()));
		}

		EntityPropertiesGuiPainter* symbolOutlineColorProp = PropertyHelper::getPainterProperty(this, "Symbol Outline Color");
		if (symbolOutlineColorProp) {
			symbolOutlineColorProp->setValue(_curve.getPointOutlinePen().getPainter());
		}

		EntityPropertiesInteger* symbolOutlineWidthProp = PropertyHelper::getIntegerProperty(this, "Symbol Outline Width");
		if (symbolOutlineWidthProp) {
			symbolOutlineWidthProp->setValue(static_cast<long>(_curve.getPointOutlinePen().getWidth()));
		}

		EntityPropertiesGuiPainter* symbolFillColorProp = PropertyHelper::getPainterProperty(this, "Symbol Fill Color");
		if (symbolFillColorProp) {
			symbolFillColorProp->setValue(_curve.getPointFillPainter());
		}
	}
	catch (...) {
		// Legacy support
	}

	this->updatePropertyVisibilities();

	m_queryInformation = _curve.getQueryInformation();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected: Virtual methods

bool EntityResult1DCurve::updatePropertyVisibilities() {
	bool visibilityChanged = false;
	
	EntityPropertiesSelection* symbolProp = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty("Symbol"));
	if (symbolProp) {
		EntityPropertiesBase* symbolSizeProp = getProperties().getProperty("Symbol Size");
		EntityPropertiesBase* symbolIntervalProp = getProperties().getProperty("Symbol Interval");
		EntityPropertiesBase* symbolOutlineColorProp = getProperties().getProperty("Symbol Outline Color");
		EntityPropertiesBase* symbolOutlineWidthProp = getProperties().getProperty("Symbol Outline Width");
		EntityPropertiesBase* symbolFillColorProp = getProperties().getProperty("Symbol Fill Color");
		
		OTAssertNullptr(symbolSizeProp);
		OTAssertNullptr(symbolIntervalProp);
		OTAssertNullptr(symbolOutlineColorProp);
		OTAssertNullptr(symbolOutlineWidthProp);
		OTAssertNullptr(symbolFillColorProp);

		bool symbolPropsVisible = symbolProp->getValue() != ot::Plot1DCurveCfg::toString(ot::Plot1DCurveCfg::NoSymbol);
		
		if (symbolSizeProp->getVisible() != symbolPropsVisible) {
			symbolSizeProp->setVisible(symbolPropsVisible);
			symbolIntervalProp->setVisible(symbolPropsVisible);
			symbolOutlineColorProp->setVisible(symbolPropsVisible);
			symbolOutlineWidthProp->setVisible(symbolPropsVisible);
			symbolFillColorProp->setVisible(symbolPropsVisible);

			visibilityChanged = true;
			getProperties().forceResetUpdateForAllProperties();
		}
	}

	return visibilityChanged;
}

void EntityResult1DCurve::addStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityBase::addStorageData(storage);

	ot::QuantityContainerEntryDescription quantityDescription = m_queryInformation.m_quantityDescription;
	bsoncxx::builder::basic::document quantityDescriptionSerialised = serialise(quantityDescription);
	
	bsoncxx::builder::basic::array arrayOfSubDocs;
	for (ot::QuantityContainerEntryDescription& parameterDescr : m_queryInformation.m_parameterDescriptions)
	{
		bsoncxx::builder::basic::document parameterDescrDoc = serialise(parameterDescr);
		arrayOfSubDocs.append(parameterDescrDoc.extract());
	}

	storage.append(
		bsoncxx::builder::basic::kvp("Query", m_queryInformation.m_query),
		bsoncxx::builder::basic::kvp("Projection", m_queryInformation.m_projection),
		bsoncxx::builder::basic::kvp("QuantityDescription", quantityDescriptionSerialised.extract()),
		bsoncxx::builder::basic::kvp("ParameterDescription", arrayOfSubDocs.extract())
	);

	
}

void EntityResult1DCurve::readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	m_queryInformation.m_query = doc_view["Query"].get_string();
	m_queryInformation.m_projection = doc_view["Projection"].get_string();

	auto& quantityDescriptionDoc =	doc_view["QuantityDescription"].get_document();

	m_queryInformation.m_quantityDescription = deserialise(quantityDescriptionDoc);

	bsoncxx::array::view parameterDescriptions	= doc_view["ParameterDescription"].get_array().value;
	
	for (auto parameterDescription = parameterDescriptions.begin(); parameterDescription != parameterDescriptions.end(); parameterDescription++)
	{
		auto& parameterDoc = parameterDescription->get_document();
		ot::QuantityContainerEntryDescription parameterDesc = deserialise(parameterDoc);
		m_queryInformation.m_parameterDescriptions.push_back(parameterDesc);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

bsoncxx::builder::basic::document EntityResult1DCurve::serialise(ot::QuantityContainerEntryDescription& _quantityContainerEntryDescription) {
	bsoncxx::builder::basic::document subDocument;

	bsoncxx::builder::basic::array quantityDimensions;
	for (uint32_t dimension : _quantityContainerEntryDescription.m_dimension)
	{
		quantityDimensions.append(static_cast<int32_t>(dimension));
	}
	subDocument.append(
		bsoncxx::builder::basic::kvp("DataType", _quantityContainerEntryDescription.m_dataType),
		bsoncxx::builder::basic::kvp("FieldName", _quantityContainerEntryDescription.m_fieldName),
		bsoncxx::builder::basic::kvp("Label", _quantityContainerEntryDescription.m_label),
		bsoncxx::builder::basic::kvp("Unit", _quantityContainerEntryDescription.m_unit),
		bsoncxx::builder::basic::kvp("Dimension", quantityDimensions)
	);
	return subDocument;
}

ot::QuantityContainerEntryDescription EntityResult1DCurve::deserialise(bsoncxx::v_noabi::document::view _subDocument) {
	ot::QuantityContainerEntryDescription quantityContainerEntryDescription;
	quantityContainerEntryDescription.m_dataType = _subDocument["DataType"].get_string();
	quantityContainerEntryDescription.m_fieldName = _subDocument["FieldName"].get_string();
	quantityContainerEntryDescription.m_label = _subDocument["Label"].get_string();
	quantityContainerEntryDescription.m_unit = _subDocument["Unit"].get_string();
	auto quantityDimension = _subDocument["Dimension"].get_array();
	for (auto& element : quantityDimension.value)
	{
		quantityContainerEntryDescription.m_dimension.push_back(static_cast<uint32_t>(element.get_int32()));
	}
	return quantityContainerEntryDescription;
}
