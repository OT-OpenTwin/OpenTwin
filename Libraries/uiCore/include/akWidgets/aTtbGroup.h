/*
 *	File:		aTtbGroup.h
 *	Package:	akWidgets
 *
 *  Created on: August 05, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 *
 *	This class is using a modified version of the "Qt TabToolbar" library.
 *	The changes to the library are listed in the documentation which was
 *	provided with this source code package.
 */

#pragma once

// AK header
#include <akCore/globalDataTypes.h>
#include <akCore/akCore.h>
#include <akGui/aTtbContainer.h>

// Qt header
#include <qstring.h>					// QString

// Forward declaration
class QAction;
namespace tt { class Group; }

namespace ak {

	// Forward declaration
	class aMessenger;
	class aUidManager;

	class UICORE_API_EXPORT aTtbGroup : public aTtbContainer {
	public:
		aTtbGroup(
			aMessenger *				_messenger,
			tt::Group *					_group,
			const QString &				_text
		);

		virtual ~aTtbGroup();

		//! @brief Will add the provided child to this container
		//! @param _child The child to add
		virtual void addChild(
			aObject *		_child
		) override;

		//! @brief Will add a new sub container to this container
		//! @param _text The initial text of the container
		virtual aTtbContainer * createSubContainer(
			const QString &				_text = QString("")
		) override;

		//! @brief Will destry all sub container created by this container
		virtual void destroyAllSubContainer(void) override;

		//! @brief Will remove the child from this object (not destroy it)
		//! This function should be called from the deconstructor of a child
		//! @param _child The child to remove
		virtual void removeChildObject(
			aObject *								_child
		) override;

		//! @brief Will set the enabled state of this container
		//! @param _enbaled The enabled state to set
		virtual void setEnabled(
			bool						_enabled
		) override;

		void addAction(QAction* _action);

	private:
		tt::Group *								m_group;

		// Block default constructor
		aTtbGroup() = delete;

		// Block copy constructor
		aTtbGroup(const aTtbGroup & _other) = delete;
		aTtbGroup & operator = (aTtbGroup &) = delete;
	};
} // namespace ak