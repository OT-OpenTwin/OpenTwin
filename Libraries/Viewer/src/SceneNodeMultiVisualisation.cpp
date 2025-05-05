#include "stdafx.h"
#include "SceneNodeMultiVisualisation.h"
#include <osg/Node>
#include <osg/Switch>
#include "TableVisualiser.h"
#include "FrontendAPI.h"

SceneNodeMultiVisualisation::~SceneNodeMultiVisualisation()
{
	//Always necessary?

	if (getShapeNode() != nullptr)
	{
		unsigned int numParents = getShapeNode()->getNumParents();

		for (unsigned int i = 0; i < numParents; i++)
		{
			osg::Group* parent = getShapeNode()->getParent(i);

			// Remove the child node from the parent (the node itself will then be deleted by reference counting automatically)
			parent->removeChild(getShapeNode());
		}

		// Now the shape node is invalid, since it might have been deleted by removing it from its parent
		m_shapeNode = nullptr;
	}
}

//ot::SelectionHandlingResult SceneNodeMultiVisualisation::setSelected(bool _selection, ot::SelectionOrigin _selectionOrigin) {
//	ot::SelectionHandlingResult result;
//
//	if (getModel() != nullptr)
//	{
//		// First we check if there is a state change
//		if (!isSelected() && _selection || isSelected() && !_selection)
//		{
//			const std::list<Visualiser*> visualisers = getVisualiser();
//			for (Visualiser* visualiser : visualisers)
//			{
//				if (_selectionOrigin == ot::SelectionOrigin::User)
//				{
//					// We have a valid state change, so we visualise all views, if they are not already opened in a view and the selection origins from a user interaction
//					// In case that properties change, effectively a new entity is created (same ID, different version) and a new scene node is created. 
//					// Therefore it is not necessary to compare the states of scenenode and entity. This algorithm only deals with the state of the view being
//					// open or not.
//
//					if (visualiser->mayVisualise() && !visualiser->viewIsCurrentlyOpen())
//					{
//						VisualiserState state;
//						state.m_selected = _selection;
//						state.m_singleSelection = getModel()->isSingleItemSelected();
//						visualiser->visualise(state);
//						result |= ot::SelectionHandlingEvent::NewViewRequested;
//					}
//					else if (visualiser->viewIsCurrentlyOpen())
//					{
//						// Here we just want to focus an already opened view.
//						FrontendAPI::instance()->setCurrentVisualizationTabFromEntityName(getName(), visualiser->getViewType());
//						result |= ot::SelectionHandlingEvent::ActiveViewChanged;
//					}
//				}
//				
//				// Other selection origins are neglected. View origin is triggered if a view is selected in UI, which triggers the selection of the corresponding entity.
//				// Custom selection changes come from somewhere within the code.		
//			}
//		}
//	}
//
//	result |= SceneNodeBase::setSelected(_selection, _selectionOrigin);
//
//	return result;
//}
