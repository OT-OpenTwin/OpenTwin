// @otlicense
// File: CheckpointRAII.h
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
