#include "stdafx.h"

#include "SceneNodeTableItem.h"

#include "DataBase.h"
#include "Model.h"
#include "Notifier.h"

#include <osg/StateSet>
#include <osg/Node>
#include <osg/BlendFunc>
#include <osg/CullFace>
#include <osg/Switch>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/PolygonOffset>
#include <osg/PolygonMode>
#include <osg/LineWidth>
#include <osg/LightModel>

SceneNodeTableItem::SceneNodeTableItem() :
	tableEntityID(0),
	tableEntityVersion(0),
	model(nullptr),
	textLoaded(false)
{

}

SceneNodeTableItem::~SceneNodeTableItem()
{
	// Remove the OSG objects 
	// This geometry node will always have a parent group or switch node

	// loop through all parent nodes
	if (getShapeNode() != nullptr)
	{
		unsigned int numParents = getShapeNode()->getNumParents();

		for (unsigned int i = 0; i < numParents; i++)
		{
			osg::Group *parent = getShapeNode()->getParent(i);

			// Remove the child node from the parent (the node itself will then be deleted by reference counting automatically)
			parent->removeChild(getShapeNode());
		}

		// Now the shape node is invalid, since it might have been deleted by removing it from its parent
		shapeNode = nullptr;
	}
}

void SceneNodeTableItem::setSelected(bool s) 
{
	if (getModel() != nullptr)
	{
		if (!isSelected() && s && getModel()->isSingleItemSelected())
		{
			// This entity is just selected -> display the table in the output window

			if (!textLoaded)
			{
				loadText();
			}

			std::string message =
				"----------------------------------------------------------------------------------------------------------------------------------\n" +
				this->getName() +
				"\n----------------------------------------------------------------------------------------------------------------------------------\n\n" +
				text +
				"\n----------------------------------------------------------------------------------------------------------------------------------\n\n";

			getNotifier()->displayText(message);
		}
	}

	SceneNodeBase::setSelected(s);
}

void SceneNodeTableItem::loadText(void)
{
	// Now load the data of the item first

	auto doc = bsoncxx::builder::basic::document{};

	if (!DataBase::GetDataBase()->GetDocumentFromEntityIDandVersion(tableEntityID, tableEntityVersion, doc))
	{
		assert(0);
		return;
	}

	auto doc_view = doc.view()["Found"].get_document().view();

	std::string entityType = doc_view["SchemaType"].get_utf8().value.data();

	if (entityType != "EntityResultTableData")
	{
		assert(0);
		return;
	}

	int schemaVersion = (int)DataBase::GetIntFromView(doc_view, "SchemaVersion_EntityResultTableData");
	if (schemaVersion != 1)
	{
		assert(0);
		return;
	}

	// Read the table
	unsigned int numberCols = doc_view["NumberColums"].get_int32();
	unsigned int numberRows = doc_view["NumberRows"].get_int32();

	auto headerArray = doc_view["Header"].get_array().value;
	auto dataArray = doc_view["Data"].get_array().value;

	std::vector<std::string> header;
	header.reserve(numberCols);

	const int minWidth = 15;

	text.clear();

	for (auto txt : headerArray)
	{
		std::string headerText = txt.get_utf8().value.data();
		if (headerText.size() < minWidth)
		{
			headerText.resize(minWidth, ' ');
		}

		header.push_back(headerText);
		text += headerText + "\t";
	}

	text += "\n";

	unsigned int iCol = 0;
	for (auto data : dataArray)
	{
		std::string value = std::to_string(data.get_double());
		value.resize(header[iCol].size(), ' ');

		text += value + "\t";

		iCol++;
		if (iCol == numberCols)
		{
			text += "\n";
			iCol = 0;
		}
	}

	textLoaded = true;
}


