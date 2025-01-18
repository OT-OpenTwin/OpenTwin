#include "stdafx.h"

#include "SceneNodeVTK.h"
#include "SceneNodeMaterial.h"
#include "Geometry.h"

#include "DataBase.h"
#include "Model.h"
#include "FrontendAPI.h"

#include <iomanip>

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
#include <osg/LightSource>
#include <osg/BlendColor>
#include <osg/Depth>

#include <osg/Image>
#include <osg/Texture2D>
#include <osgDB/ReadFile>

SceneNodeVTK::SceneNodeVTK() :
	model(nullptr),
	initialized(false),
	dataID(0),
	dataVersion(0)
{

}


SceneNodeVTK::~SceneNodeVTK()
{
	// Remove the OSG objects 
	// This geometry node will always have a parent group or switch node

	deleteShapeNode();
}

void SceneNodeVTK::deleteShapeNode(void)
{
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

void SceneNodeVTK::setTransparent(bool t)
{
	if (getShapeNode() == nullptr) return; 

	if (isTransparent() == t) return;  // No change necessary

	SceneNodeBase::setTransparent(t);

	if (isTransparent())
	{
		// Set node to transparent state
		osg::StateSet *stateset = getShapeNode()->getOrCreateStateSet();
		stateset->clear();

		osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc;
		blendFunc->setFunction(GL_CONSTANT_COLOR, GL_ZERO);
		stateset->setAttributeAndModes(blendFunc);

		osg::ref_ptr<osg::BlendColor> blend = new osg::BlendColor();
		blend->setConstantColor(osg::Vec4(0.25, 0.25, 0.25, 1.0));
		stateset->setAttributeAndModes(blend, osg::StateAttribute::ON);

		stateset->setRenderingHint(osg::StateSet::OPAQUE_BIN);  // We can not render the vtka images transparent, since they have their own transparency settings inside. 
															    // Thereforen, we will just dim them when they are unselected.

		//getShapeNode()->setNodeMask(getShapeNode()->getNodeMask() & ~1);  // Reset last bit of node mask
	}
	else
	{
		// Set node to opaque state
		osg::StateSet *stateset = getShapeNode()->getOrCreateStateSet();
		stateset->clear();

		//getShapeNode()->setNodeMask(getShapeNode()->getNodeMask() | 1);  // Set last bit of node mask
	}
}

void SceneNodeVTK::setWireframe(bool w)
{
	if (getShapeNode() == nullptr) return;

	if (isWireframe() == w) return;  // No change necessary

	SceneNodeBase::setWireframe(w);

	/*
	double lineWidth = 0.0;	
	
	if (isWireframe())
	{
		// Set node to wireframe state
		// Turn off display of triangles (if the shape is not transparent)

		if (!isTransparent())
		{
			getShapeNode()->setChildValue(getTriangles(), false);
		}

		lineWidth = 2.5;
		
	}
	else
	{
		// Set node to solid state
		// Turn on display of triangles (if the shape is not hidden or transparent)

		if (isVisible() && !isTransparent())
		{
			getShapeNode()->setChildValue(getTriangles(), true);
		}

		lineWidth = 1.0;
	}

	// Find the line width attribute of the edges and set the width
	osg::StateSet *ss = getEdges()->getOrCreateStateSet();

	// find the line with attribute of the state set
	osg::LineWidth *lineWidthAttribute = dynamic_cast<osg::LineWidth*>(ss->getAttribute(osg::StateAttribute::Type::LINEWIDTH));

	if (lineWidthAttribute != nullptr)
	{
		lineWidthAttribute->setWidth(lineWidth);
	}
	*/
}

void SceneNodeVTK::setVisible(bool v)
{
	if (isVisible() == v) return;  // No change necessary

	if (v && !initialized)
	{
		if (dataID == 0)
		{
			// Here we need to send a message to the model for updating the item data
			FrontendAPI::instance()->updateVTKEntity(getModelEntityID());
		}
		else
		{
			osg::Node *vtkNode = createOSGNodeFromVTK();

			if (vtkNode == nullptr)
			{
				// This entity might have been deleted and needs to be regenerated
				// This will also an update message to be queued which will then update the entity in a next step.
				FrontendAPI::instance()->updateVTKEntity(getModelEntityID());
			}
			else
			{
				shapeNode->addChild(vtkNode);
				initialized = true;
			}
		}
	}

	SceneNodeBase::setVisible(v);

	if (getShapeNode() != nullptr)
	{
		if (isVisible())
		{
			getShapeNode()->setAllChildrenOn();
		}
		else
		{
			getShapeNode()->setAllChildrenOff();
		}
	}
}

void SceneNodeVTK::setHighlighted(bool h)
{
	if (getShapeNode() == nullptr) return;

	if (isHighlighted() == h) return;  // No change necessary

	SceneNodeBase::setHighlighted(h);
}

void SceneNodeVTK::updateVTKNode(const std::string &projName, unsigned long long visualizationDataID, unsigned long long visualizationDataVersion)
{
	dataID      = visualizationDataID;
	dataVersion = visualizationDataVersion;
	projectName = projName;

	// Add a switch (group) node for the shape
	if (shapeNode == nullptr)
	{
		// Create an new shapeNode
		shapeNode = new osg::Switch;

		// Now add the current nodes osg node to the parent's osg node
		getParent()->getShapeNode()->addChild(getShapeNode());

		getModel()->addSceneNode(this);
	}
	else
	{
		// Delete the children of the shape node
		while (shapeNode->removeChild((unsigned int)0));
	}

	initialized = false;
	
	if (isVisible())
	{
		osg::Node *vtkNode = createOSGNodeFromVTK();

		if (vtkNode == nullptr)
		{
			// This entity might have been deleted and needs to be regenerated
			// This will also an update message to be queued which will then update the entity in a next step.
			FrontendAPI::instance()->updateVTKEntity(getModelEntityID());
		}
		else
		{
			shapeNode->addChild(vtkNode);
			initialized = true;
		}
	}
}

std::string SceneNodeVTK::loadDataItem(unsigned long long entityID, unsigned long long entityVersion)
{
	auto doc = bsoncxx::builder::basic::document{};

	if (!DataBase::GetDataBase()->GetDocumentFromEntityIDandVersion(entityID, entityVersion, doc))
	{
		// assert(0); This can happen, if the entity is deleted
		return "";
	}

	auto doc_view = doc.view()["Found"].get_document().view();

	std::string entityType = doc_view["SchemaType"].get_utf8().value.data();

	if (entityType != "EntityBinaryData")
	{
		assert(0);
		return "";
	}

	int schemaVersion = (int)DataBase::GetIntFromView(doc_view, "SchemaVersion_EntityBinaryData");
	if (schemaVersion != 1)
	{
		assert(0);
		return "";
	}

	// Now load the actual data

	auto data = doc_view["data"].get_binary();

	std::string stringData;
	stringData.reserve(data.size);

	for (unsigned long index = 0; index < data.size; index++)
	{
		stringData.push_back(data.bytes[index]);
	}

	return stringData;
}

osg::Node *SceneNodeVTK::createOSGNodeFromVTK(void)
{
	if (dataID == 0)
	{
		return nullptr;
	}

	// Load the binary entity and create the node string
	std::list< std::pair<unsigned long long, unsigned long long>> dataList;
	dataList.push_back(std::pair<unsigned long long, unsigned long long>(dataID, dataVersion));

	DataBase::GetDataBase()->PrefetchDocumentsFromStorage(dataList);

	std::string dataString = loadDataItem(dataID, dataVersion);

	if (dataString.empty())
	{
		// The entity was deleted from the data base and needs to be re-generated
		return nullptr;
	}

	// Convert the node string to an osg node

	std::stringstream dataIn(dataString);

	osg::Node *node = nullptr;

	osgDB::ReaderWriter* rw2 = osgDB::Registry::instance()->getReaderWriterForExtension("osgb");
	if (rw2)
	{
		osgDB::ReaderWriter::ReadResult rr = rw2->readNode(dataIn);
		node = rr.takeNode();
	}

	// return the new osg node
	return node;
}

void SceneNodeVTK::reportTime(const std::string &message, std::time_t &timer)
{
	double seconds = difftime(time(nullptr), timer);

	std::stringstream msg;
	msg << message << ": " << std::setprecision(6) << seconds << " sec." << std::endl;

	FrontendAPI::instance()->displayText(msg.str());

	timer = time(nullptr);
}
