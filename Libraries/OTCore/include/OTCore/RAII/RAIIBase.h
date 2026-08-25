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

		RAIIBase& operator=(RAIIBase&& _other) noexcept = delete;

	public:

		//! @brief Apply and invalidate.
		void finalize();

		//! @brief Invalidate.
		void dismiss();

		virtual bool isValid() const { return m_isValid; };

	protected:
		virtual void execute() = 0;
		virtual void invalidate() {};
		
	private:
		bool m_isValid;
	};

}