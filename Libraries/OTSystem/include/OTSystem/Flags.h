// @otlicense
// File: Flags.h
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
#include "OTSystem/ArchitectureInfo.h"

// std header
#include <type_traits>

//! @brief Adds bitwise operations for the provided enum.
//! Use convenience OT_ADD_FLAG_FUNCTIONS or OT_ADD_FRIEND_FLAG_FUNCTIONS for most cases.
//! @note Only enumeration types are allowed.
//! @param ___prefix Prefix for the functions, e.g. 'friend'.
//! @param ___enumName Name of the enum.
//! @param ___flagsType Name of the Flags typedef for the enum.
#define OT_ADD_FLAG_FUNCTIONS_IMPL(___prefix, ___enumName, ___flagsType) ___prefix constexpr ___flagsType operator | (___enumName _lhv, ___enumName _rhv) { return ___flagsType(_lhv) | _rhv; }; \
___prefix constexpr ___flagsType operator & (___enumName _lhv, ___enumName _rhv) { return ___flagsType(_lhv) & _rhv; };  \
___prefix constexpr ___flagsType operator ^ (___enumName _lhv, ___enumName _rhv) { return ___flagsType(_lhv) ^ _rhv; };  \
___prefix constexpr ___flagsType operator ~ (___enumName _lhv) { return ~(___flagsType(_lhv)); };

//! @def OT_ADD_FLAG_FUNCTIONS
//! @brief Add default bitwise operations (global) for the provided enum bitfield.
//! Use this if the enum is in a public scope.
//!
//! Example:
//!     namespace MyNamespace {
//!         enum MyEnum : unsigned {
//!				EmptyFlags = 0,
//!				A = 1 << 0,
//!				B = 1 << 1 
//!			};
//!         typedef ot::Flags<MyEnum> MyFlags;
//!     }
//!
//! Then in an implementation file (or header) call:
//!     OT_ADD_FLAG_FUNCTIONS(MyNamespace::MyEnum, MyNamespace::MyFlags)
#define OT_ADD_FLAG_FUNCTIONS(___enumName, ___flagsType) OT_ADD_FLAG_FUNCTIONS_IMPL( , ___enumName, ___flagsType)

//! @def OT_ADD_FRIEND_FLAG_FUNCTIONS
//! @brief Add default bitwise operations as friends (use inside class scope).
//! Use this inside the class scope where the enum is defined if the enum is private.
//! (The generated functions are declared `friend` so they can be found via ADL.)
//! @param ___enumName Name of the enum.
//! @param ___flagsName Name of the Flags type.
#define OT_ADD_FRIEND_FLAG_FUNCTIONS(___enumName, ___flagsType) OT_ADD_FLAG_FUNCTIONS_IMPL(friend, ___enumName, ___flagsType)

namespace ot {

	//! @class Flags
	//! @brief Lightweight wrapper that enables bitwise flag semantics for enum types.
	//!
	//! Requirements & notes:
	//!  - T must be an enumeration type.
	//!  - The enum should define values that represent single bits or masks.
	//!  - Converting back to T (enum) when the stored integer contains bit patterns
	//!    that do not correspond to a named enumerator is implementation-defined; use
	//!    `toEnum()` only when that is acceptable.
	//!
	//! Example:
	//!     enum MyEnum : uint64_t {
	//!         Empty = 0,
	//!         V1    = 1 << 0,
	//!         V2    = 1 << 1,
	//!         Mask  = V1 | V2
	//!     };
	//!     typedef ot::Flags<MyEnum> MyFlags;
	//! @tparam T Enumeration type.
	//! @tparam Storage Underlying storage type (default: std::uint64_t).
	template<typename T, typename Storage = std::uint64_t> class Flags {
	private:
		static_assert(std::is_enum<T>::value, "Flags require an enumeration type T.");

		using Underlying = typename std::underlying_type<T>::type;
		using UnsignedUnderlying = typename std::make_unsigned<Underlying>::type;

		static_assert(sizeof(Underlying) <= sizeof(Storage), "Underlying enum type must fit into the chosen Storage type.");

		Storage m_data; //! @brief Data.

		//! @brief Helper to cast enum to Storage (via underlying).
		//! @param _v Enum value.
		static constexpr Storage toStorage(T _v) noexcept {
			return static_cast<Storage>(static_cast<UnsignedUnderlying>(static_cast<Underlying>(_v)));
		}

		//! @brief Helper to cast Storage to enum (via underlying).
		//! @param _s Storage value.
		static constexpr T fromStorage(Storage _s) noexcept {
			return static_cast<T>(static_cast<Underlying>(_s));
		}

	public:
		//! @brief Default constructor.
		//! Initializes the data with 0.
		constexpr Flags() noexcept : m_data{ 0 } {};

		//! @brief Assignment constructor.
		//! @param _initialData Initial data.
		constexpr Flags(T _initialData) noexcept : m_data{ toStorage(_initialData) } {};

		//! @brief Assignment constructor.
		//! @param _initialData Initial data.
		explicit constexpr Flags(Storage _raw) noexcept : m_data{ _raw } {};

		//! @brief Copy constructor.
		//! @param _other Other flags.
		constexpr Flags(const Flags& _other) noexcept = default;

		//! @brief Move constructor.
		//! @param _other Other flags.
		constexpr Flags(Flags&& _other) noexcept = default;

		//! @brief Destructor.
		~Flags() = default;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Conversion

		//! @brief Get raw storage value.
		constexpr Storage underlying() const noexcept { return m_data; };

		//! @brief Returns the stored flags as enum value.
		//! @note Use with care. Converting back to T (enum) may be implementation-defined.
		constexpr T toEnum() const noexcept { return fromStorage(m_data); };

		//! @brief Explicit conversion to enum type.
		//! @note Use with care. Converting back to T (enum) may be implementation-defined.
		explicit constexpr operator T() const noexcept { return fromStorage(m_data); };

		//! @brief Returns true if any bit is set.
		constexpr bool any() const noexcept { return m_data != Storage{ 0 }; };

		//! @brief Returns true if no bit is set.
		constexpr bool none() const noexcept { return m_data == Storage{ 0 }; };

		//! @brief Conversion to bool operator (same as calling any()).
		constexpr operator bool() const { return static_cast<bool>(m_data); };

		//! @brief Conversion to underlying storage type (same as calling underlying).
		explicit constexpr operator Storage() const { return m_data; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Modifiers

		//! @brief Set the provided flag
		//! @param _flag The flag to set
		inline void set(T _flag) noexcept { m_data |= toStorage(_flag); };

		//! @brief Set or remove the provided flag
		//! @param _flag The flag to set
		//! @param _flagIsSet If true the flag will be set, otherwise removed
		inline void set(T _flag, bool _flagIsSet) noexcept {
			if (_flagIsSet) {
				m_data |= toStorage(_flag);
			}
			else {
				m_data &= ~(toStorage(_flag));
			}
		};

		//! @brief Remove the provided flag
		//! @param _flag The flag to remove
		inline void remove(T _flag) noexcept { m_data &= ~(toStorage(_flag)); };

		//! @brief Clears all flags (i.e. sets the data to 0).
		inline void clear() noexcept { m_data = Storage{ 0 }; };

		inline Flags& operator = (T _flag) noexcept { m_data = toStorage(_flag); return *this; };
		inline Flags& operator = (const Flags& _other) noexcept = default;
		inline Flags& operator = (Flags&& _other) noexcept = default;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Tests / Comparison

		//! @brief Returns true if *all* bits in mask are set.
		constexpr bool has(T _mask) const noexcept {
			return (m_data & toStorage(_mask)) == toStorage(_mask);
		};

		//! @brief Returns true if *any* bit in mask is set.
		constexpr bool hasAny(T _mask) const noexcept {
			return (m_data & toStorage(_mask)) != Storage{ 0 };
		};

		constexpr bool operator == (T _flag) const noexcept { return m_data == toStorage(_flag); };
		constexpr bool operator == (const Flags& _other) const noexcept { return m_data == _other.m_data; };

		constexpr bool operator != (T _flag) const noexcept { return m_data != toStorage(_flag); };
		constexpr bool operator != (const Flags& _other) const noexcept { return m_data != _other.m_data; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Bitwise operators

		constexpr Flags operator | (T _flag) const noexcept { return Flags{ m_data | toStorage(_flag) }; };
		constexpr Flags operator | (const Flags& _other) const noexcept { return Flags{ m_data | _other.m_data }; };

		constexpr Flags operator & (T _flag) const noexcept { return Flags{ m_data & toStorage(_flag) }; };
		constexpr Flags operator & (const Flags& _other) const noexcept { return Flags{ m_data & _other.m_data }; };

		constexpr Flags operator^(T _flag) const noexcept { return Flags{ m_data ^ toStorage(_flag) }; };
		constexpr Flags operator^(const Flags& _other) const noexcept { return Flags{ m_data ^ _other.m_data }; };

		inline Flags& operator |= (T _flag) noexcept { m_data |= toStorage(_flag); return *this; };
		inline Flags& operator |= (const Flags& _other) noexcept { m_data |= _other.m_data; return *this; };

		inline Flags& operator &= (T _flag) noexcept { m_data &= toStorage(_flag); return *this; };
		inline Flags& operator &= (const Flags& _other) noexcept { m_data &= _other.m_data; return *this; };

		inline Flags& operator^=(T _flag) noexcept { m_data ^= toStorage(_flag); return *this; }
		inline Flags& operator^=(const Flags& _other) noexcept { m_data ^= _other.m_data; return *this; }

		constexpr Flags operator ~() const noexcept { return Flags{ ~m_data }; };

	};

}