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
