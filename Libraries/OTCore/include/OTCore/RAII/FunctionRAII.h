// @otlicense
// File: FunctionRAII.h
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

// std header
#include <functional>

namespace ot {

    //! @brief The FunctionRAII may be used to impletement any basic lifecycle management for a given object.
    //! Based on the <a href="https://en.cppreference.com/w/cpp/language/raii">RAII pattern</a>. <br>
    //! 
    //! Example:
    //! 
    //! @code
    //! #include "OTCore/RAII/FunctionRAII.h"
    //! 
    //! void foo(void) {
    //!     int* myNumber = nullptr;
    //!     ot::FunctionRAII<int*> num(
    //!         myNumber,                 // Will be stored and passed to the two provided methods.
    //!         [](int*& _lcl){           // When creating "num" myNumber will be initialized with a new pointer.
    //!             _lcl = new int;
    //!             assert(_lcl != nullptr);
    //!         },
    //!         [](int*&){ delete _lcl; } // The destructor will destroy myNumber.
    //!     );
    //! 
    //!     // Now the value was created.
    //! 
    //!     // Do anything with myNumber or use num.getValue() to access the pointer.
    //!     ...
    //! }
    //! @endcode
    class FunctionRAII : public RAIIBase
    {
        OT_DECL_NOCOPY(FunctionRAII)
        OT_DECL_NODEFAULT(FunctionRAII)
    public:
        //! @brief Constructor.
        //! @param _value Value reference will be stored and has to remain valid while the wrapper instance is not destroyed.
        //! @param _onCreate Function that should be called when the wrapper instance is created.
        //! @param _onDelete Function that should be called when the wrapper instance is destroyed.
        explicit FunctionRAII(std::function<void()> _onCreate, std::function<void()> _onDelete)
            : m_onDelete(_onDelete)
        {
            if (_onCreate) {
                _onCreate();
            }
        }

        FunctionRAII(FunctionRAII&& _other) noexcept
            : RAIIBase(std::move(_other)), m_onDelete(std::move(_other.m_onDelete))
        {
            _other.m_onDelete = nullptr;
		}

        //! @brief Destructor.
        //! If the FunctionRAII#m_onDelete function was set it will be called.
        virtual ~FunctionRAII() {
            if (static_cast<bool>(m_onDelete) && this->isValid()) {
                m_onDelete();
            }
        }

        FunctionRAII& operator=(FunctionRAII&& _other) noexcept
        {
            if (this != &_other)
            {
                RAIIBase::operator=(std::move(_other));
                m_onDelete = std::move(_other.m_onDelete);
                _other.m_onDelete = nullptr;
            }
            return *this;
		}

        void dismiss()
        {
            this->invalidate();
        }

        void execute()
        {
            if (static_cast<bool>(m_onDelete) && this->isValid())
            {
                m_onDelete();
                this->invalidate();
            }
		}

        constexpr inline const std::function<void()>& getOnDeleteFunction() const { return m_onDelete; };

    protected:
        void invalidate() override {
            RAIIBase::invalidate();
            m_onDelete = nullptr;
        }

    private:
        std::function<void()> m_onDelete; //! @brief Function that will be called in the destructor.

    };

}
