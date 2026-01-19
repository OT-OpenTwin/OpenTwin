// @otlicense
// File: DecrementRAII.h
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

namespace ot {

    //! @class DecrementRAII
    //! @brief Wrapper increments a value on creation and decrements the value when the instance is destroyed.
    //! Based on the <a href="https://en.cppreference.com/w/cpp/language/raii">RAII pattern</a>.
    template <typename T> class DecrementRAII {
        OT_DECL_NOCOPY(DecrementRAII)
        OT_DECL_NOMOVE(DecrementRAII)
        OT_DECL_NODEFAULT(DecrementRAII)

        static_assert(
            std::is_same<decltype(std::declval<T&>() += static_cast<T>(1)), T&>::value&&
            std::is_same<decltype(std::declval<T&>() -= static_cast<T>(1)), T&>::value,
            "T must support += and -= with value 1"
        );

    public:
        //! @brief Constructor.
        //! Increments the value by one. The value will be decremented on destruction.
        //! @param _value Value reference will be stored and has to remain valid while the wrapper instance is not destroyed.
        explicit DecrementRAII(T& _value) : m_value(_value) {
            m_value -= static_cast<T>(1);
        }

        //! @brief Destructor.
        //! Decrements the value by one.
        ~DecrementRAII() {
            m_value += static_cast<T>(1);
        }

    private:
        T& m_value;
    };

}
