// @otlicense

// AK header
#include <akCore/aException.h>
#include <akCore/aMessenger.h>
#include <akGui/aTtbContainer.h>

ak::aTtbContainer::aTtbContainer(
	aMessenger *			_messenger,
	objectType				_type,
	UID						_uid
) : aObject(_type, _uid),
	m_messenger(nullptr),
	m_isEnabled(true)
{
	assert(_messenger != nullptr); // Is nullptr
	m_messenger = _messenger;
}

ak::aTtbContainer::~aTtbContainer() {}

ak::aTtbContainer * ak::aTtbContainer::getSubContainer(
	const QString &				_text
) {
	for (auto itm : m_subContainer) { if (itm->text() == _text) { return itm; } }
	return nullptr;
}

int ak::aTtbContainer::subContainerCount(void) const { return m_subContainer.size(); }
