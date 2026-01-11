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
#include "OTCore/CoreTypes.h"

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
    template <typename T> class FunctionRAII {
        OT_DECL_NOCOPY(FunctionRAII)
        OT_DECL_NOMOVE(FunctionRAII)
        OT_DECL_NODEFAULT(FunctionRAII)
    public:
        //! @brief Constructor.
        //! @param _value Value reference will be stored and has to remain valid while the wrapper instance is not destroyed.
        //! @param _onCreate Function that should be called when the wrapper instance is created.
        //! @param _onDelete Function that should be called when the wrapper instance is destroyed.
        explicit FunctionRAII(T& _value, std::function<void(T&)> _onCreate, std::function<void(T&)> _onDelete)
            : m_value(_value), m_onDelete(_onDelete)
        {
            if (_onCreate) {
                _onCreate(m_value);
            }
        }

        //! @brief Destructor.
        //! If the FunctionRAII#m_onDelete function was set it will be called.
        ~FunctionRAII() {
            if (m_onDelete) {
                m_onDelete(m_value);
            }
        }

        constexpr inline T& getValueRef() const { return m_value; };
        constexpr inline const std::function<void(T&)>& getOnDeleteFunction() const { return m_onDelete; };

    private:
        T& m_value; //! @brief Value reference.
        std::function<void(T&)> m_onDelete; //! @brief Function that will be called in the destructor.

    };

}
