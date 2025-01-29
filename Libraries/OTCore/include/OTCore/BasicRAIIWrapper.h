//! @file BasicRAIIWrapper.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#include <iostream>
#include <functional>

namespace ot {

    template <typename T>
    class BasicRAIIWrapper {
    public:
        BasicRAIIWrapper() = delete;
        BasicRAIIWrapper(T& _value, std::function<void(T&)> _onEnter, std::function<void(T&)> _onExit)
            : m_value(_value), m_onExit(_onExit)
        {
            if (_onEnter) {
                _onEnter(m_value);
            }
        }

        ~BasicRAIIWrapper() {
            if (m_onExit) {
                m_onExit(m_value);
            }
        }

        BasicRAIIWrapper(const BasicRAIIWrapper&) = delete;
        BasicRAIIWrapper& operator = (const BasicRAIIWrapper&) = delete;
        BasicRAIIWrapper(BasicRAIIWrapper&&) = default;
        BasicRAIIWrapper& operator = (BasicRAIIWrapper&&) = default;

        T& getValue(void) const { return m_value; };

    private:
        T& m_value;
        std::function<void(T&)> m_onExit;
    };

}
