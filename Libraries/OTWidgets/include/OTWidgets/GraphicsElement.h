//! @file GraphicsElement.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtCore/qpoint.h>

// std header
#include <list>

#pragma warning(disable:4251)

class QGraphicsItem;

namespace ot {

	class GraphicsScene;

	class OT_WIDGETS_API_EXPORT GraphicsElement {
		OT_DECL_NOCOPY(GraphicsElement)
		OT_DECL_NOMOVE(GraphicsElement)
	public:
		//! @enum GraphicsElementState
		//! @brief The GraphicsElementState is used to describe the current state of a GraphicsElement.
		enum GraphicsElementState {
			NoState         = 0 << 0, //! @brief Default state.
			HoverState      = 1 << 0, //! @brief Item is hovered over by user.
			SelectedState   = 1 << 1, //! @brief Item is selected.
			DestroyingState = 1 << 2  //! @brief Item is being destroyed.
		};
		//! @typedef GraphicsItemStateFlags
		//! @brief Flags used to manage GraphicsItemState.
		//! @see GraphicsItem, GraphicsItemState
		typedef Flags<GraphicsElementState> GraphicsElementStateFlags;

		GraphicsElement();
		virtual ~GraphicsElement();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Public helper

		//! @brief Calculates and returns the shortest distance to the given point.
		//! Returns -1 if the distance is invalid (e.g. maximum distance exceeded).
		//! @param _pt Point in scene coordinates.
		virtual qreal calculateShortestDistanceToPoint(const QPointF& _pt) const { return -1.; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		//! @brief Returns the QGraphicsItem.
		virtual QGraphicsItem* getQGraphicsItem() = 0;

		//! @brief Returns the const QGraphicsItem.
		virtual const QGraphicsItem* getQGraphicsItem() const = 0;

		//! @brief Set the GraphicsScene this element is placed at.
		void setGraphicsScene(GraphicsScene* _scene);

		//! @brief Returns the GraphicsScene this item is placed at.
		virtual GraphicsScene* getGraphicsScene() const { return m_scene; };

		//! @brief Sets the provided state flag.
		//! @see GraphicsItem, GraphicsItemState
		//! @param _state The state to set.
		//! @param _active If true the flag will be set, otherwise unset.
		void setGraphicsElementState(GraphicsElementState _state, bool _active = true);

		//! @brief Replaces the flags with the flags provided.
		//! @param _flags Flags to set.
		void setGraphicsElementStateFlags(const GraphicsElementStateFlags& _state);

		//! @brief Sets the DestroyingState flag.
		void setGraphicsElementDestroying() { this->setGraphicsElementState(DestroyingState, true); };

		//! @brief Returns the current GraphicsItemStateFlags set.
		//! @see GraphicsItem, GraphicsItemStateFlags
		const GraphicsElementStateFlags& getGraphicsElementState() const { return m_state; };

		//! @brief Returns all graphics elements nested in this element.
		//! The result contains this element and all of its childs.
		virtual std::list<GraphicsElement*> getAllGraphicsElements();

		virtual std::list<GraphicsElement*> getAllDirectChildElements();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Protected notifier

	protected:
		virtual void graphicsSceneSet(GraphicsScene* _scene) {};
		virtual void graphicsSceneRemoved() {};
		virtual void graphicsElementStateChanged(const GraphicsElementStateFlags& _state) {};

	private:
		GraphicsScene* m_scene;
		GraphicsElementStateFlags m_state;
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::GraphicsElement::GraphicsElementState)