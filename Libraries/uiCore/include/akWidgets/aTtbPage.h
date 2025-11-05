// @otlicense
// File: aTtbPage.h
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

// TTB header
#include <TabToolbar/Page.h>

// Qt header
#include <qstring.h>					// QString

// Forward declaraion

namespace ak {

	// Forward declaration
	class aMessenger;

	class UICORE_API_EXPORT aTtbPage : public aTtbContainer {
	public:
		aTtbPage(
			aMessenger *				_messenger,
			tt::Page *					_page,
			const QString &				_text
		);

		virtual ~aTtbPage();

		//! @brief Will add the provided child to this container
		//! @param _child The child to add
		virtual void addChild(
			aObject *					_child
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
			aObject *					_child
		) override;

		//! @brief Will return the index of this page
		int index(void) const;

		//! @brief Will set the enabled state of this container
		//! @param _enbaled The enabled state to set
		virtual void setEnabled(
			bool						_enabled
		) override;

		virtual QWidget* widget(void) override { return m_page; }

		tt::Page* getPage(void) const { return m_page; };

	private:
		tt::Page *						m_page;

		// Block default constructor
		aTtbPage() = delete;

		// Block copy constructor
		aTtbPage(const aTtbPage & _other) = delete;
		aTtbPage& operator = (aTtbPage&) = delete;
	};
} // namespace ak