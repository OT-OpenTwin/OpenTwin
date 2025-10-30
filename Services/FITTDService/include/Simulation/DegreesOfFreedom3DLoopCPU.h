// @otlicense
// File: DegreesOfFreedom3DLoopCPU.h
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
#include "SystemDependencies/SystemDependentDefines.h"
#include "SystemDependencies/Allignments.h"
#include <malloc.h>

template<class T>
class DegreesOfFreedom3DLoopCPU
{
private:
	T * _electricVoltageXComponent = nullptr;
	T * _electricVoltageYComponent = nullptr;
	T * _electricVoltageZComponent = nullptr;
	T * _magneticVoltageXComponent = nullptr;
	T * _magneticVoltageYComponent = nullptr;
	T * _magneticVoltageZComponent = nullptr;

	Alignment _activeAlignment = CacheLine64;

public:
	DegreesOfFreedom3DLoopCPU(const index_t numberOfDoF, Alignment _alignment = CacheLine64);
	~DegreesOfFreedom3DLoopCPU();

	inline T* GetElectricVoltageXComponent(index_t index) const;
	inline T* GetElectricVoltageYComponent(index_t index) const;
	inline T* GetElectricVoltageZComponent(index_t index) const;

	inline T* GetMagneticVoltageXComponent(index_t index) const;
	inline T* GetMagneticVoltageYComponent(index_t index) const;
	inline T* GetMagneticVoltageZComponent(index_t index) const;

	inline void SetElectricVoltageXComponentAt(index_t index, T value);
	inline void SetElectricVoltageYComponentAt(index_t index, T value);
	inline void SetElectricVoltageZComponentAt(index_t index, T value);

	inline void SetMagneticVoltageXComponentAt(index_t index, T value);
	inline void SetMagneticVoltageYComponentAt(index_t index, T value);
	inline void SetMagneticVoltageZComponentAt(index_t index, T value);

};


template<class T>
inline DegreesOfFreedom3DLoopCPU<T>::DegreesOfFreedom3DLoopCPU(const index_t numberOfDoF, Alignment alignment)
{
	if (alignment != AVX512 && alignment != AVX2)
	{
		_activeAlignment = CacheLine64;
	}
	else
	{
		_activeAlignment = alignment;
	}
	
	_electricVoltageXComponent = static_cast<T*>(_aligned_malloc(numberOfDoF * sizeof(T), _activeAlignment));
	_electricVoltageYComponent = static_cast<T*>(_aligned_malloc(numberOfDoF * sizeof(T), _activeAlignment));
	_electricVoltageZComponent = static_cast<T*>(_aligned_malloc(numberOfDoF * sizeof(T), _activeAlignment));
	_magneticVoltageXComponent = static_cast<T*>(_aligned_malloc(numberOfDoF * sizeof(T), _activeAlignment));
	_magneticVoltageYComponent = static_cast<T*>(_aligned_malloc(numberOfDoF * sizeof(T), _activeAlignment));
	_magneticVoltageZComponent = static_cast<T*>(_aligned_malloc(numberOfDoF * sizeof(T), _activeAlignment));
	
	if (_electricVoltageXComponent == nullptr || _electricVoltageYComponent == nullptr || _electricVoltageZComponent == nullptr||
		_magneticVoltageXComponent == nullptr || _magneticVoltageYComponent == nullptr || _magneticVoltageZComponent == nullptr)  
	{
		throw std::exception("Error while allocating aligned memory.");
	}

	for (index_t i = 0; i < numberOfDoF; i++)
	{
		_electricVoltageXComponent[i]=0;
		_electricVoltageYComponent[i]=0;
		_electricVoltageZComponent[i]=0;
		_magneticVoltageXComponent[i]=0;
		_magneticVoltageYComponent[i]=0;
		_magneticVoltageZComponent[i]=0;
	}
}

template<class T>
inline DegreesOfFreedom3DLoopCPU<T>::~DegreesOfFreedom3DLoopCPU()
{
	_aligned_free(_electricVoltageXComponent);
	_aligned_free(_electricVoltageYComponent);
	_aligned_free(_electricVoltageZComponent);
	_aligned_free(_magneticVoltageXComponent);
	_aligned_free(_magneticVoltageYComponent);
	_aligned_free(_magneticVoltageZComponent);

	_electricVoltageXComponent = nullptr;
	_electricVoltageYComponent = nullptr;
	_electricVoltageZComponent = nullptr;
	_magneticVoltageXComponent = nullptr;
	_magneticVoltageYComponent = nullptr;
	_magneticVoltageZComponent = nullptr;
}

template<class T>
inline T* DegreesOfFreedom3DLoopCPU<T>::GetElectricVoltageXComponent(index_t index) const
{
	return &_electricVoltageXComponent[index];
}

template<class T>
inline T* DegreesOfFreedom3DLoopCPU<T>::GetElectricVoltageYComponent(index_t index) const
{
	return &_electricVoltageYComponent[index];
}

template<class T>
inline T* DegreesOfFreedom3DLoopCPU<T>::GetElectricVoltageZComponent(index_t index) const
{
	return &_electricVoltageZComponent[index];
}

template<class T>
inline T* DegreesOfFreedom3DLoopCPU<T>::GetMagneticVoltageXComponent(index_t index) const
{
	return &_magneticVoltageXComponent[index];
}

template<class T>
inline T* DegreesOfFreedom3DLoopCPU<T>::GetMagneticVoltageYComponent(index_t index) const
{
	return &_magneticVoltageYComponent[index];
}

template<class T>
inline T* DegreesOfFreedom3DLoopCPU<T>::GetMagneticVoltageZComponent(index_t index) const
{
	return &_magneticVoltageZComponent[index];
}

template<class T>
inline void DegreesOfFreedom3DLoopCPU<T>::SetElectricVoltageXComponentAt(index_t index, T value)
{
	_electricVoltageXComponent[index] = value;
}

template<class T>
inline void DegreesOfFreedom3DLoopCPU<T>::SetElectricVoltageYComponentAt(index_t index, T value)
{
	_electricVoltageYComponent[index] = value;
}

template<class T>
inline void DegreesOfFreedom3DLoopCPU<T>::SetElectricVoltageZComponentAt(index_t index, T value)
{
	_electricVoltageZComponent[index] = value;
}

template<class T>
inline void DegreesOfFreedom3DLoopCPU<T>::SetMagneticVoltageXComponentAt(index_t index, T value)
{
	_magneticVoltageXComponent[index] = value;
}

template<class T>
inline void DegreesOfFreedom3DLoopCPU<T>::SetMagneticVoltageYComponentAt(index_t index, T value)
{
	_magneticVoltageYComponent[index] = value;
}

template<class T>
inline void DegreesOfFreedom3DLoopCPU<T>::SetMagneticVoltageZComponentAt(index_t index, T value)
{
	_magneticVoltageZComponent[index] = value;
}




