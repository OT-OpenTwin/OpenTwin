//! @file GraphicsElement.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Flags.h"
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qpoint.h>

// std header
#include <list>

#pragma warning(disable:4251)

namespace ot {

	class GraphicsScene;

	class OT_WIDGETS_API_EXPORT GraphicsElement {
		OT_DECL_NOCOPY(GraphicsElement)
	public:
		//! \enum GraphicsElementState
		//! \brief The GraphicsElementState is used to describe the current state of a GraphicsElement.
		enum GraphicsElementState {
			NoState = 0x00, //! \brief Default state.
			HoverState = 0x01, //! \brief Item is hovered over by user.
			SelectedState = 0x02, //! \brief Item is selected.
		};
		//! \typedef GraphicsItemStateFlags
		//! \brief Flags used to manage GraphicsItemState.
		//! \see GraphicsItem, GraphicsItemState
		typedef Flags<GraphicsElementState> GraphicsElementStateFlags;

		GraphicsElement();
		virtual ~GraphicsElement();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Public helper

		//! \brief Calculates and returns the shortest distance to the given point.
		//! Returns -1 if the distance is invalid (e.g. maximum distance exceeded).
		//! \param _pt Point in scene coordinates.
		virtual qreal calculateShortestDistanceToPoint(const QPointF& _pt) const { return -1.; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		//! \brief Set the GraphicsScene this element is placed at.
		virtual void setGraphicsScene(GraphicsScene* _scene) { m_scene = _scene; };

		//! \brief Returns the GraphicsScene this item is placed at.
		virtual GraphicsScene* getGraphicsScene(void) const { return m_scene; };

		//! \brief Sets the provided state flag.
		//! \see GraphicsItem, GraphicsItemState
		//! \param _state The state to set.
		//! \param _active If true the flag will be set, otherwise unset.
		void setGraphicsElementState(GraphicsElementState _state, bool _active = true);

		//! \brief Replaces the flags with the flags provided.
		//! \param _flags Flags to set.
		void setGraphicsElementStateFlags(const GraphicsElementStateFlags& _state);

		//! \brief Returns the current GraphicsItemStateFlags set.
		//! \see GraphicsItem, GraphicsItemStateFlags
		const GraphicsElementStateFlags& getGraphicsElementState(void) const { return m_state; };

		//! \brief Returns all graphics elements nested in this element.
		//! The result contains this element and all of its childs.
		virtual std::list<GraphicsElement*> getAllGraphicsElements(void);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Protected notifier

	protected:
		virtual void graphicsElementStateChanged(const GraphicsElementStateFlags& _state) {};

	private:
		GraphicsScene* m_scene;
		GraphicsElementStateFlags m_state;
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::GraphicsElement::GraphicsElementState)