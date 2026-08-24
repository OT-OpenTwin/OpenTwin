// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"

namespace ot
{
	class OT_CORE_API_EXPORT RAIIBase
	{
		OT_DECL_NOCOPY(RAIIBase)
	protected:
		explicit RAIIBase();
		explicit RAIIBase(RAIIBase&& _other) noexcept : m_isValid(_other.m_isValid) { _other.m_isValid = false; };
		virtual ~RAIIBase() = default;

		RAIIBase& operator=(RAIIBase&& _other) noexcept {
			if (this != &_other) {
				m_isValid = _other.m_isValid;
				_other.m_isValid = false;
			}
			return *this;
		}

	public:
		inline bool isValid() const { return m_isValid; };

	protected:
		virtual void invalidate() { m_isValid = false; };
		
	private:
		bool m_isValid;
	};

}