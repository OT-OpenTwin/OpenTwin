// @otlicense
// File: LogMessageStream.hpp
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
#include "OTCore/Logging/LogMessageStream.h"

template<class T, std::enable_if_t<!std::is_base_of<ot::Serializable, std::remove_cv_t<T>>::value, int>>
inline ot::LogMessageStream& ot::LogMessageStream::operator<<(T* _ptr)
{
    return *this << Pointer(static_cast<const void*>(_ptr));
}

template <typename T, typename>
inline ot::LogMessageStream& ot::LogMessageStream::operator<<(const std::set<T>& _set)
{
	appendList(_set);
    return *this;
}

template <typename T, typename>
inline ot::LogMessageStream& ot::LogMessageStream::operator<<(const std::unordered_set<T>& _set)
{
    appendList(_set);
    return *this;
}

template <typename T, typename>
inline ot::LogMessageStream& ot::LogMessageStream::operator<<(const std::list<T>& _list)
{
    appendList(_list);
    return *this;
}

template <typename T, typename>
inline ot::LogMessageStream& ot::LogMessageStream::operator<<(const std::vector<T>& _vector)
{
    appendList(_vector);
    return *this;
}

template <typename K, typename V, typename, typename>
inline ot::LogMessageStream& ot::LogMessageStream::operator<<(const std::map<K, V>& _map)
{
    appendMap(_map);
    return *this;
}

template <typename K, typename V, typename, typename>
inline ot::LogMessageStream& ot::LogMessageStream::operator<<(const std::unordered_map<K, V>& _map)
{
    appendMap(_map);
    return *this;
}

template <typename T>
void ot::LogMessageStream::appendList(const T& _list)
{
    m_stream << "[ ";
    bool first = true;

    for (const auto& item : _list)
    {
        if (!first)
        {
            m_stream << ", ";
        }

        *this << item;
        first = false;
    }

    m_stream << " ]";
}

template <typename T>
void ot::LogMessageStream::appendMap(const T& _list)
{
    m_stream << "[ ";
    bool first = true;

    for (const auto& item : _list)
    {
        if (!first)
        {
            m_stream << ", ";
        }

		m_stream << "{ ";
		
        *this << item.first;
		m_stream << ", ";
		*this << item.second;

		m_stream << " }";

        first = false;
    }

    m_stream << " ]";
}