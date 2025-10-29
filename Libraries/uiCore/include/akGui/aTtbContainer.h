// @otlicense

/*
 *	File:		aTtbContainer.h
 *	Package:	akGui
 *
 *  Created on: August 05, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

// C++ header
#include <vector>

// Qt header
#include <qstring.h>

// AK header
#include <akCore/globalDataTypes.h>
#include <akCore/akCore.h>
#include <akCore/aObject.h>

#define TTB_CONT_DEL_PARENTCHECK assert(m_parentObject != nullptr); m_parentObject->removeChildObject(this);
#define TTB_CONTAINER_DESTROYING TTB_CONT_DEL_PARENTCHECK

namespace ak {

	// Forward declaration
	class aMessenger;
	class aUidManager;
	
	//! This class provides a interface that represents a tab toolbar container.
	class UICORE_API_EXPORT aTtbContainer : public aObject {
	public:

		//! @brief Constructor, initializes the uid of this container
		//! @param _UID The initial UID of this object
		//! @param _references The initial reference count for this object
		aTtbContainer(
			aMessenger *			_messenger,
			objectType				_type,
			UID						_UID = ak::invalidUID
		);

		//! @brief Deconstructor
		virtual ~aTtbContainer();

		//! @brief Returns the count of sub containers
		int subContainerCount(void) const;

		//! @brief Will add the provided child to this container
		//! @param _child The child to add
		virtual void addChild(
			aObject *				_child
		) = 0;

		//! @brief Will add a new sub container to this container
		//! @param _text The initial text of the container
		virtual aTtbContainer * createSubContainer(
			const QString &			_text = QString("")
		) = 0;
		
		//! @brief Will set the enabled state of this container
		//! @param _enbaled The enabled state to set
		virtual void setEnabled(
			bool					_enabled
		) { m_isEnabled = _enabled; }

		//! @brief Will return the enabled state of this container
		bool enabled(void) const { return m_isEnabled; }

		//! @brief Will return the sub container with the specified text
		//! Returns nullptr if no sub container with the specified text exists
		//! @param _text The text of the sub container to find
		aTtbContainer * getSubContainer(
			const QString &				_text
		);

		virtual void destroyAllSubContainer(void) = 0;

		//! @brief Will return the text of this ttb container
		QString text(void) const { return m_text; }

	protected:
		aMessenger *					m_messenger;
		std::vector<aTtbContainer *>	m_subContainer;
		QString							m_text;
		bool							m_isEnabled;

	private:
		// Block default constructor
		aTtbContainer() = delete;

		// Block copy constructor
		aTtbContainer(const aTtbContainer & _other) = delete;

		// Block assignment operator
		aTtbContainer & operator = (const aTtbContainer & _other) = delete;
	};
} // namespace ak