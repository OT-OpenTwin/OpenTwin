// @otlicense
// File: FlagSetRAII.h
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
            _other.dismiss();
		}

        //! @brief Destructor.
        //! Decrements the value by one.
        virtual ~FlagSetRAII()
        {
            this->finalize();
        }

        FlagSetRAII& operator=(FlagSetRAII&&) noexcept = delete;

        virtual bool isValid() const override
        {
            return (m_flags != nullptr) && RAIIBase::isValid();
        }

    protected:
        virtual void execute() override
        {
            m_flags->remove(m_reset);
        }

        virtual void invalidate() override
        {
            RAIIBase::invalidate();
            m_flags = nullptr;
        }
        
    private:
        Flags<T>* m_flags;
		Flags<T> m_reset;
    };

}
