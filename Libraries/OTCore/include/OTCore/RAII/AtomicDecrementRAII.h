// @otlicense
// File: AtomicDecrementRAII.h
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
#include <atomic>

namespace ot {

    //! @class AtomicDecrementRAII
    //! @brief Wrapper increments a value on creation and decrements the value when the instance is destroyed.
    //! Based on the <a href="https://en.cppreference.com/w/cpp/language/raii">RAII pattern</a>.
    template <typename T> class AtomicDecrementRAII {
        OT_DECL_NOCOPY(AtomicDecrementRAII)
        OT_DECL_NOMOVE(AtomicDecrementRAII)
        OT_DECL_NODEFAULT(AtomicDecrementRAII)

        static_assert(
            std::is_arithmetic_v<T>,
            "T must be an arithmetic type"
        );

    public:
        //! @brief Constructor.
        //! Increments the value by one. The value will be decremented on destruction.
        //! @param _value Value reference will be stored and has to remain valid while the wrapper instance is not destroyed.
        explicit AtomicDecrementRAII(std::atomic<T>& _value, std::memory_order _order = std::memory_order_seq_cst) : m_value(_value), m_order(_order) {
            m_value.store(m_value.load() - static_cast<T>(1), m_order);
        }

        //! @brief Destructor.
        //! Decrements the value by one.
        ~AtomicDecrementRAII() {
            m_value.store(m_value.load() + static_cast<T>(1), m_order);
        }

    private:
        std::atomic<T>& m_value;
        std::memory_order m_order;
    };

}
