// @otlicense

#pragma once

// OpenTwin header
#include "OTGui/GuiTypes.h"
#include "OTGui/OTGuiAPIExport.h"

#pragma warning(disable : 4251)

namespace ot {

	class OT_GUI_API_EXPORT KeySequence {
	public:
		KeySequence();
		KeySequence(BasicKey _key);
		KeySequence(BasicKey _key, BasicKey _key2);
		KeySequence(BasicKey _key, BasicKey _key2, BasicKey _key3);
		KeySequence(BasicKey _key, BasicKey _key2, BasicKey _key3, BasicKey _key4);
		KeySequence(const std::list<BasicKey>& _keys);
		~KeySequence();

		operator std::string(void) const;

	private:
		std::list<BasicKey> m_keys;

	};

}