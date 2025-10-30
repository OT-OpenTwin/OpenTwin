// @otlicense
// File: ContainerFlexibleOwnership.h
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
template <class T>
class ContainerFlexibleOwnership
{
public:
	ContainerFlexibleOwnership(size_t _size);
	~ContainerFlexibleOwnership();
	ContainerFlexibleOwnership(ContainerFlexibleOwnership&& _other)
	{
		m_data = _other.m_data;
		_other.m_data = nullptr;
	}
	ContainerFlexibleOwnership& operator=(const ContainerFlexibleOwnership& _other)
	{
		m_data = _other.m_data;
		_other.m_data = nullptr;
	}
	ContainerFlexibleOwnership(const ContainerFlexibleOwnership& _other) = delete;
	ContainerFlexibleOwnership& operator=(ContainerFlexibleOwnership&& _other) = default;

	const T& operator[](size_t _index);
	void pushBack(const T& _value);
	T* release();
private:
	T* m_data = nullptr;
	size_t m_numberOfEntries;
	size_t m_pushBackPtr = 0;
};

template<class T>
inline ContainerFlexibleOwnership<T>::ContainerFlexibleOwnership(size_t _size)
	:m_numberOfEntries(_size)
{
	m_data = new T[m_numberOfEntries];
}

template<class T>
inline ContainerFlexibleOwnership<T>::~ContainerFlexibleOwnership()
{
	if (m_data != nullptr)
	{
		delete[] m_data;
		m_data = nullptr;
	}
}

template<class T>
inline const T& ContainerFlexibleOwnership<T>::operator[](size_t _index)
{
	return m_data[_index];
}

template<class T>
inline void ContainerFlexibleOwnership<T>::pushBack(const T& _value)
{
	if (m_pushBackPtr >= m_numberOfEntries)
	{
		throw std::out_of_range("Index out of bounds");
	}
	m_data[m_pushBackPtr] = _value;
	m_pushBackPtr++;
}

template<class T>
inline T* ContainerFlexibleOwnership<T>::release()
{
	T* temp = m_data;
	m_data = nullptr;
	return temp;
}
