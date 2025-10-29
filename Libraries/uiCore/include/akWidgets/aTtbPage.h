// @otlicense

#pragma once

 // AK header
#include <akCore/globalDataTypes.h>
#include <akCore/akCore.h>
#include <akGui/aTtbContainer.h>

// Qt header
#include <qstring.h>					// QString

// Forward declaraion
namespace tt { class Page; }

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