/*
 *	File:		aTtbSubgroup.cpp
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

 // AK header
#include <akCore/aException.h>
#include <akCore/aMessenger.h>
#include <akCore/aUidMangager.h>

#include <akGui/aAction.h>

#include <akWidgets/aTtbSubGroup.h>
#include <akWidgets/aWidget.h>

 // TTB header (TabToolbar library)
#include <TabToolbar/SubGroup.h>		// tt::Subgroup

ak::aTtbSubGroup::aTtbSubGroup(
	aMessenger *				_messenger,
	tt::SubGroup *				_group,
	const QString &				_text
) : ak::aTtbContainer(_messenger, otTabToolbarSubgroup), m_subGroup(_group)
{
	assert(m_subGroup != nullptr); // Nullptr provided
	assert(_messenger != nullptr); // Nullptr provided
	m_text = _text;
	m_subGroup->setObjectName("OT_TTBSubGroup");
}

ak::aTtbSubGroup::~aTtbSubGroup() {
	TTB_CONTAINER_DESTROYING
		
	delete m_subGroup;
}

void ak::aTtbSubGroup::addChild(
	ak::aObject *		_child
) {
	assert(_child != nullptr); // Nullptr provided
	if (_child->type() == otAction) {
		// Cast action
		aAction * ac = nullptr;
		ac = dynamic_cast<aAction *>(_child);
		assert(ac != nullptr); // Cast failed
		//Place action
		m_subGroup->AddAction(ac->popupMode(), ac);
	}
	else {
		// Check child
		assert(_child->isWidgetType()); // Provided object is no action and no widget
		// Cast widget
		aWidget * w = nullptr;
		w = dynamic_cast<aWidget *>(_child);
		assert(w != nullptr); // Cast failed
		// Place widget
		m_subGroup->AddWidget(w->widget());
	}
	_child->setParentObject(this);
	addChildObject(_child);
	// Store object
	m_childObjects.insert_or_assign(_child->uid(), _child);
}

ak::aTtbContainer * ak::aTtbSubGroup::createSubContainer(
	const QString &				_text
) {
	assert(0); // Cannot add a sub container to a sub group
	return nullptr;
}

void ak::aTtbSubGroup::destroyAllSubContainer(void) {}

void ak::aTtbSubGroup::removeChildObject(
	aObject *								_child
) {
	assert(_child != nullptr); // Nullptr provided
	ak::aObject::removeChildObject(_child);
	
	if (_child->type() == otAction) {
		aAction * action = nullptr;
		action = dynamic_cast<aAction *>(_child);
		assert(action != nullptr);	// Cast failed
		m_subGroup->removeAction(action);
	}
}

void ak::aTtbSubGroup::setEnabled(
	bool						_enabled
) {
	aTtbContainer::setEnabled(_enabled);
	m_subGroup->setEnabled(m_isEnabled);
}