// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/RAII/RAIIBase.h"

namespace ot
{

    //! @class CheckpointRAII
    //! Based on the <a href="https://en.cppreference.com/w/cpp/language/raii">RAII pattern</a>.
    template <typename T> class CheckpointRAII : public RAIIBase
    {
        OT_DECL_NOCOPY(CheckpointRAII)
        OT_DECL_NODEFAULT(CheckpointRAII)
    public:
        explicit CheckpointRAII(T& _value) : m_value(&_value), m_reset(_value) {};

        CheckpointRAII(CheckpointRAII&& _other) noexcept : RAIIBase(std::move(_other)), m_value(_other.m_value), m_reset(std::move(_other.m_reset))
        {
			_other.m_value = nullptr;
		}

        //! @brief Destructor.
        //! Decrements the value by one.
        virtual ~CheckpointRAII()
        {
            this->execute();
        }

        CheckpointRAII& operator=(CheckpointRAII&& _other) noexcept
        {
            if (this != &_other)
            {
                RAIIBase::operator=(std::move(_other));

                m_value = _other.m_value;
                m_reset = std::move(_other.m_reset);

                _other.m_value = nullptr;
            }

            return *this;
        }

        void dismiss() {
            this->invalidate();
		}

        void execute() {
            if (m_value && this->isValid())
            {
                *m_value = m_reset;
                this->invalidate();
            }
		}

	protected:
        void invalidate() override {
            RAIIBase::invalidate();
            m_value = nullptr;
		}

    private:
        T* m_value;
		T m_reset;
    };

}
