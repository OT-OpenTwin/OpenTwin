// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/RAII/RAIIBase.h"

namespace ot
{

	//! @class FlagSetRAII
    //! Based on the <a href="https://en.cppreference.com/w/cpp/language/raii">RAII pattern</a>.
    template <typename T> class FlagSetRAII : public RAIIBase
    {
        OT_DECL_NOCOPY(FlagSetRAII)
        OT_DECL_NODEFAULT(FlagSetRAII)

    public:
        
		//! @brief Constructor.
		//! @param _flags Flags reference will be stored and has to remain valid while the wrapper instance is not destroyed.
		//! @param _set Flags to set on creation.
        explicit FlagSetRAII(Flags<T>& _flags, const Flags<T>& _set) : m_flags(&_flags), m_reset(_set)
        {
            m_flags->set(_set);
        }

        FlagSetRAII(FlagSetRAII&& _other) noexcept : RAIIBase(std::move(_other)), m_flags(_other.m_flags), m_reset(std::move(_other.m_reset))
        {
			_other.m_flags = nullptr;
		}

        //! @brief Destructor.
        //! Decrements the value by one.
        virtual ~FlagSetRAII()
        {
            execute();
        }

        FlagSetRAII& operator=(FlagSetRAII&& _other) noexcept
        {
            if (this != &_other)
            {
                RAIIBase::operator=(std::move(_other));
                m_flags = _other.m_flags;
				m_reset = std::move(_other.m_reset);
                _other.m_flags = nullptr;
            }
            return *this;
		}

        void dismiss()
        {
            invalidate();
		}

        void execute()
        {
            if (m_flags && this->isValid())
            {
                m_flags->remove(m_reset);
                invalidate();
			}
        }

    protected:
        void invalidate() override
        {
            RAIIBase::invalidate();
            m_flags = nullptr;
		}

    private:
        Flags<T>* m_flags;
		Flags<T> m_reset;
    };

}
