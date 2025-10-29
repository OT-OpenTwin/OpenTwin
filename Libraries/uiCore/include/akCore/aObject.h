// @otlicense

/*
 *	File:		aObject.h
 *	Package:	akCore
 *
 *  Created on: July 26, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

// AK header
#include <akCore/globalDataTypes.h>
#include <akCore/akCore.h>

// Qt header
#include <qstring.h>				// QString

// C++ header
#include <map>

//! Use to notify the parent object that this object is currently destroying
#define A_OBJECT_DESTROYING_WITHOWNER if (m_owner != nullptr) { ak::aObject * obj = m_owner->parentObject(); if (obj != nullptr) { obj->removeChildObject(m_owner); } }
#define A_OBJECT_DESTROYING_WITHPARENT if (m_parentObject != nullptr) { m_parentObject->removeChildObject(this); m_parentObject = nullptr; }
#define A_OBJECT_DESTROYING A_OBJECT_DESTROYING_WITHOWNER else A_OBJECT_DESTROYING_WITHPARENT

namespace ak {

	//! This class is used to store the main information of any object used
	//! Information contained is the UID and the reference counter
	class UICORE_API_EXPORT aObject {
	public:

		//! @brief Constructor
		//! @param _UID The initial UID of this object
		//! @param _references The initial reference count for this object
		aObject(
			objectType				_type = otNone,
			UID						_UID = ak::invalidUID
			//int										_references = 1
		);

		//! @brief Copy constructor
		//! @param _other The other object
		aObject(
			const aObject &			_other
		);

		//! @brief Assignment operator
		//! @param _other The other object
		aObject & operator = (
			const aObject &					_other
			);

		//! @brief Deconstructor
		virtual ~aObject();

		// ################################################################################

		//! @brief Will remove the child from this object (not destroy it)
		//! This function should be called from the deconstructor of a child
		//! @param _child The child to remove
		virtual void removeChildObject(
			aObject *								_child
		);

		//! @brief Will add the child to this object
		//! @param _child The child to add
		virtual void addChildObject(
			aObject *								_child
		);

		//! @brief Will set this objects unique name
		//! @param _name The name of this object
		void setUniqueName(
			const QString &							_name
		) { m_uniqueName = _name; }

		//! @brief Will set the parent object of this object
		virtual void setParentObject(
			aObject *								_parentObject
		);

		// ################################################################################

		//! @brief Will return the unique name of this object
		QString uniqueName(void) const { return m_uniqueName; }

		//! @brief Will return a pointer to the parent object
		aObject * parentObject(void) const;

		//! @brief Will return the count of sub objects
		int childObjectCount(void) const;

		//! @brief Will return the child with the specified UID
		//! @param _childUID The UID of the child object
		aObject * childObject(
			UID									_childUID
		);

		//! @brief Will set the owner of this object
		//! @param _object The to set as owner
		void setOwner(
			aObject *								_object
		);

		//! @brief Will return the owner of this object
		aObject * owner(void) const;

		// ################################################################################

		//! @brief Will set the objects UID
		void setUid(
			UID									_UID
		);

		//! @brief Returns the objects UID
		UID uid(void) const;

		//! @brief Returns the objects type
		objectType type(void) const;

		//! @brief Returns true if the object is an object derived from aWidget
		virtual bool isWidgetType(void) const;

	protected:
		UID							m_uid;				//! The objects UID
		int							m_references;		//! The objects references
		objectType					m_objectType;		//! The object type of this object
		QString						m_uniqueName;
		aObject *					m_parentObject;
		aObject *					m_owner;
		std::map<UID, aObject *>	m_childObjects;
	};
}