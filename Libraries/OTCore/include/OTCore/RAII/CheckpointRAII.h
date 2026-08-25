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
            _other.dismiss();
		}

        //! @brief Destructor.
        //! Decrements the value by one.
        virtual ~CheckpointRAII()
        {
            this->finalize();
        }

        CheckpointRAII& operator=(CheckpointRAII&&) noexcept = delete;

        virtual bool isValid() const override
        {
            return (m_value != nullptr) && RAIIBase::isValid();
		}

    protected:
        virtual void execute() override
        {
            *m_value = m_reset;
        }

        virtual void invalidate() override
        {
			RAIIBase::invalidate();
			m_value = nullptr;
        }

    private:
        T* m_value;
		T m_reset;
    };

}
