#include "EntityResult1DCurve.h"
#include "PropertyHelper.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCore/EntityName.h"
#include "OTGui/Painter2D.h"
#include "OTGui/ColorStyleTypes.h"
#include "OTGui/StyleRefPainter2D.h"
#include "OTGui/VisualisationTypes.h"
#include "OTGui/Painter2DDialogFilterDefaults.h"

EntityResult1DCurve::EntityResult1DCurve(ot::UID _ID, EntityBase* _parent, EntityObserver* _mdl, ModelState* _ms, ClassFactoryHandler* _factory, const std::string& _owner)
	: EntityBase(_ID,_parent,_mdl,_ms,_factory,_owner)
{
}

void EntityResult1DCurve::addVisualizationNodes(void)
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

bool EntityResult1DCurve::updateFromProperties(void)
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UpdateCurvesOfPlot, doc.GetAllocator()), doc.GetAllocator());
	
	const std::string plotName = getParent()->getName();
	doc.AddMember(OT_ACTION_PARAM_NAME, ot::JsonString(plotName, doc.GetAllocator()), doc.GetAllocator());

	ot::JsonObject curveCfgSerialised;
	ot::Plot1DCurveCfg curveCfg = getCurve();
	curveCfg.addToJsonObject(curveCfgSerialised, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_VIEW1D_CurveConfigs, curveCfgSerialised,doc.GetAllocator());

	getObserver()->sendMessageToViewer(doc);
	return false;
}

void EntityResult1DCurve::createProperties(void)
{
	EntityPropertiesGuiPainter* colorProp = EntityPropertiesGuiPainter::createProperty("General", "Color", new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::PlotCurve), "", getProperties());
	colorProp->setFilter(ot::Painter2DDialogFilterDefaults::plotCurve());

	EntityPropertiesInteger* widthProp = EntityPropertiesInteger::createProperty("General", "Line Width", 1, 1, 99, "", getProperties());
	widthProp->setAllowCustomValues(false);

	EntityPropertiesSelection* selectionProp = EntityPropertiesSelection::createProperty("General", "Line Style", {
		ot::toString(ot::LineStyle::SolidLine),
		ot::toString(ot::LineStyle::DashLine),
		ot::toString(ot::LineStyle::DotLine),
		ot::toString(ot::LineStyle::DashDotLine),
		ot::toString(ot::LineStyle::DashDotDotLine)
		},
		ot::toString(ot::LineStyle::SolidLine), "", getProperties());

	getProperties().forceResetUpdateForAllProperties();
}

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
	}
	catch (...) {}
	try {
		penCfg.setStyle(ot::stringToLineStyle(PropertyHelper::getSelectionPropertyValue(this, "Line Style")));
	}
	catch (...) {}
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

void EntityResult1DCurve::setCurve(const ot::Plot1DCurveCfg& _curve) {
	PropertyHelper::setPainterPropertyValue(_curve.getLinePen().getPainter(), this, "Color");
	
	try {
		EntityPropertiesInteger* prop = PropertyHelper::getIntegerProperty(this, "Line Width");
		if (prop) {
			prop->setValue(static_cast<long>(_curve.getLinePen().getWidth()));
		}
	}
	catch (...) {}

	try {
		EntityPropertiesSelection* prop = PropertyHelper::getSelectionProperty(this, "Line Style");
		if (prop) {
			prop->setValue(ot::toString(_curve.getLinePen().getStyle()));
		}
	}
	catch (...) {}

	m_queryInformation = _curve.getQueryInformation();
}

void EntityResult1DCurve::AddStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityBase::AddStorageData(storage);

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

bsoncxx::builder::basic::document EntityResult1DCurve::serialise(ot::QuantityContainerEntryDescription& _quantityContainerEntryDescription)
{
	bsoncxx::builder::basic::document subDocument;
	subDocument.append(
		bsoncxx::builder::basic::kvp("DataType", _quantityContainerEntryDescription.m_dataType),
		bsoncxx::builder::basic::kvp("FieldName", _quantityContainerEntryDescription.m_fieldName),
		bsoncxx::builder::basic::kvp("Label", _quantityContainerEntryDescription.m_label),
		bsoncxx::builder::basic::kvp("Unit", _quantityContainerEntryDescription.m_unit)
	);
	return subDocument;
}


ot::QuantityContainerEntryDescription EntityResult1DCurve::deserialise(bsoncxx::v_noabi::document::view _subDocument)
{
	ot::QuantityContainerEntryDescription quantityContainerEntryDescription;
	quantityContainerEntryDescription.m_dataType = _subDocument["DataType"].get_string();
	quantityContainerEntryDescription.m_fieldName = _subDocument["FieldName"].get_string();
	quantityContainerEntryDescription.m_label = _subDocument["Label"].get_string();
	quantityContainerEntryDescription.m_unit = _subDocument["Unit"].get_string();
	return quantityContainerEntryDescription;
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
