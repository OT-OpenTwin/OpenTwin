// @otlicense
// File: aTtbGroup.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once

// AK header
#include <akCore/globalDataTypes.h>
#include <akCore/akCore.h>
#include <akGui/aTtbContainer.h>

// Qt header
#include <qstring.h>					// QString

// TTB header
#include <TabToolbar/Group.h>

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

		virtual QWidget* widget(void) override { return m_group; }

		tt::Group* getGroup() { return m_group; };

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