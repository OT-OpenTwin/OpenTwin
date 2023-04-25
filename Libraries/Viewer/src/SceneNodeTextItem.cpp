#include "stdafx.h"

#include "SceneNodeTextItem.h"

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

SceneNodeTextItem::SceneNodeTextItem() :
	textEntityID(0),
	textEntityVersion(0),
	model(nullptr),
	textLoaded(false)
{

}

SceneNodeTextItem::~SceneNodeTextItem()
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

void SceneNodeTextItem::setSelected(bool s) 
{
	if (getModel() != nullptr)
	{
		if (!isSelected() && s && getModel()->isSingleItemSelected())
		{
			// This entity is just selected -> display its text in the output window

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

void SceneNodeTextItem::loadText(void)
{
	// Now load the data of the item first

	auto doc = bsoncxx::builder::basic::document{};

	if (!DataBase::GetDataBase()->GetDocumentFromEntityIDandVersion(textEntityID, textEntityVersion, doc))
	{
		assert(0);
		return;
	}

	auto doc_view = doc.view()["Found"].get_document().view();

	std::string entityType = doc_view["SchemaType"].get_utf8().value.data();

	if (entityType != "EntityResultTextData")
	{
		assert(0);
		return;
	}

	int schemaVersion = (int)DataBase::GetIntFromView(doc_view, "SchemaVersion_EntityResultTextData");
	if (schemaVersion != 1)
	{
		assert(0);
		return;
	}

	// Read the text
	text = doc_view["textData"].get_utf8().value.data();
	textLoaded = true;
}

