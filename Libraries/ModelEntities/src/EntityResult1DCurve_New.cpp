#include "EntityResult1DCurve_New.h"
#include "PropertyHelper.h"
#include "OTCommunication/ActionTypes.h"
#include "OTGui/Painter2D.h"
#include "OTGui/VisualisationTypes.h"

EntityResult1DCurve_New::EntityResult1DCurve_New(ot::UID _ID, EntityBase* _parent, EntityObserver* _mdl, ModelState* _ms, ClassFactoryHandler* _factory, const std::string& _owner)
	: EntityBase(_ID,_parent,_mdl,_ms,_factory,_owner)
{
}

void EntityResult1DCurve_New::addVisualizationNodes(void)
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

bool EntityResult1DCurve_New::updateFromProperties(void)
{
	return false;
}

void EntityResult1DCurve_New::createProperties(void)
{
	EntityPropertiesColor::createProperty("General", "Color", { 255, 0, 0 }, "", getProperties());
	EntityPropertiesString::createProperty("X axis", "X axis label", "", "", getProperties());
	EntityPropertiesString::createProperty("X axis", "X axis unit", "", "", getProperties());
	EntityPropertiesString::createProperty("Y axis", "Y axis label", "", "", getProperties());
	EntityPropertiesString::createProperty("Y axis", "Y axis unit", "", "", getProperties());

	PropertyHelper::getStringProperty(this, "X axis label")->setReadOnly(true);
	PropertyHelper::getStringProperty(this, "X axis unit")->setReadOnly(true);
	PropertyHelper::getStringProperty(this, "Y axis label")->setReadOnly(true);
	PropertyHelper::getStringProperty(this, "Y axis unit")->setReadOnly(true);

	getProperties().forceResetUpdateForAllProperties();
}

ot::Plot1DCurveCfg EntityResult1DCurve_New::getCurve()
{
	ot::Plot1DCurveCfg curveCfg;

	curveCfg.setEntityName(this->getName());

	const ot::Color colour = PropertyHelper::getColourPropertyValue(this, "Color");

	const std::string xAxisLabel =PropertyHelper::getStringPropertyValue(this, "X axis label");
	const std::string xAxisUnit =PropertyHelper::getStringPropertyValue(this, "X axis unit");

	const std::string yAxisLabel =PropertyHelper::getStringPropertyValue(this, "Y axis label");
	const std::string yAxisUnit =PropertyHelper::getStringPropertyValue(this, "Y axis unit");
	
	curveCfg.setLinePenColor(colour);
	curveCfg.setXAxisTitle(xAxisLabel);
	curveCfg.setXAxisUnit(xAxisUnit);

	curveCfg.setYAxisTitle(yAxisLabel);
	curveCfg.setYAxisUnit(yAxisUnit);

	curveCfg.setQueryInformation(m_queryInformation);
	return curveCfg;
}

void EntityResult1DCurve_New::setCurve(const ot::Plot1DCurveCfg& _curve)
{
	PropertyHelper::setColourPropertyValue(_curve.getLinePen().painter()->getDefaultColor(), this, "Color");

	PropertyHelper::setStringPropertyValue(_curve.getXAxisTitle(), this, "X axis label");
	PropertyHelper::setStringPropertyValue(_curve.getXAxisUnit(), this, "X axis unit");

	PropertyHelper::setStringPropertyValue(_curve.getYAxisTitle(), this, "Y axis label");
	PropertyHelper::setStringPropertyValue(_curve.getYAxisUnit(), this, "Y axis unit");

	m_queryInformation = _curve.getQueryInformation();
}

void EntityResult1DCurve_New::AddStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityBase::AddStorageData(storage);

	storage.append(
		bsoncxx::builder::basic::kvp("Query", m_queryInformation.m_query),
		bsoncxx::builder::basic::kvp("Projection", m_queryInformation.m_projection),
		bsoncxx::builder::basic::kvp("QuantityFieldName", m_queryInformation.m_quantityFieldName),
		bsoncxx::builder::basic::kvp("ParameterFieldName", m_queryInformation.m_parameterFieldName)
	);
}

void EntityResult1DCurve_New::readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);
	m_queryInformation.m_query = doc_view["Query"].get_string();
	m_queryInformation.m_projection = doc_view["Projection"].get_string();
	m_queryInformation.m_quantityFieldName = doc_view["QuantityFieldName"].get_string();
	m_queryInformation.m_parameterFieldName= doc_view["ParameterFieldName"].get_string();

}
