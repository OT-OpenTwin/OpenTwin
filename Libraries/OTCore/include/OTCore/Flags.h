#pragma once

// OpenTwin header
#include "OTSystem/ArchitectureInfo.h"

// std header
#include <type_traits>

#ifdef OT_OS_64Bit

//! @brief Will add the default bitwise operations for the provided bitfield
#define OT_ADD_FLAG_FUNCTIONS(___enumName) inline ___enumName operator | (___enumName _lhv, ___enumName _rhv) { return (___enumName)((long long) _lhv | (long long)_rhv); }; \
inline ___enumName & operator |= (___enumName & _lhv, ___enumName _rhv) { return (_lhv = (___enumName)((long long) _lhv | (long long)_rhv)); }; \
inline ___enumName operator & (___enumName _lhv, ___enumName _rhv) { return (___enumName)((long long) _lhv & (long long)_rhv); };  \
inline ___enumName & operator &= (___enumName & _lhv, ___enumName _rhv) { return (_lhv = (___enumName)((long long) _lhv & (long long)_rhv)); };  \
inline ___enumName operator ~ (___enumName _lhv) { return (___enumName)(~((long long) _lhv)); }; \
inline bool operator == (___enumName _lhv, const ot::Flags<___enumName>& _rhv) { return ((long long)_lhv == (long long)_rhv.data()); };
#else
//! @brief Will add the default bitwise operations for the provided bitfield
#define OT_ADD_FLAG_FUNCTIONS(___enumName) inline ___enumName operator | (___enumName _lhv, ___enumName _rhv) { return (___enumName)((long) _lhv | (long)_rhv); }; \
inline ___enumName & operator |= (___enumName & _lhv, ___enumName _rhv) { return (_lhv = (___enumName)((long) _lhv | (long)_rhv)); }; \
inline ___enumName operator & (___enumName _lhv, ___enumName _rhv) { return (___enumName)((long) _lhv & (long)_rhv); };  \
inline ___enumName & operator &= (___enumName & _lhv, ___enumName _rhv) { return (_lhv = (___enumName)((long) _lhv & (long)_rhv)); };  \
inline ___enumName operator ~ (___enumName _lhv) { return (___enumName)(~((long) _lhv)); }; \
inline bool operator == (___enumName _lhv, const ot::Flags<___enumName>& _rhv) { return ((long)_lhv == (long)_rhv.data()); };
#endif

namespace ot {

	//! @brief This class is used to manage flags.
	//! Don't forget to add OT_ADD_FLAG_FUNCTIONS and the bottom of your header.
	//! The type should be an enumeration where every value represents a single bit in a 32/64 bit value
	//! e.g:
	//! enum enumName {
	//!		enumValue1	= 0x01,	//	0001
	//!		enumValue2	= 0x02,	//	0010
	//!		enumValue3	= 0x04,	//	0100
	//!			...
	//!	}
	template<typename T> class Flags {
	private:
#ifdef OT_OS_64Bit
		typedef long long FlagCastType;
#else
		typedef long FlagCastType;
#endif // _WIN64

		static_assert((sizeof(T) <= sizeof(FlagCastType)), "Flags type may overflow.");
		static_assert((std::is_enum<T>::value), "Flags accepts only enumeration types.");

		T m_data;

	public:
		constexpr inline Flags() : m_data{ static_cast<T>(0) } {};
		constexpr inline Flags(T _initialData) : m_data{ _initialData } {}
		constexpr inline Flags(const Flags<T>& _other) : m_data{ _other.data() } {};

		//! @brief Returns a copy of the data
		constexpr inline T data(void) const { return m_data; };

		//! @brief Replace the current data
		//! @param _data The data that should be replaced with
		constexpr inline void set(T _data) { m_data = _data; };

		//! @brief Set the provided flag
		//! @param _flag The flag to set
		constexpr inline void setFlag(T _flag) { m_data |= _flag; };

		//! @brief Set or remove the provided flag
		//! @param _flag The flag to set
		//! @param _flagIsSet If true the flag will be set, otherwise removed
		constexpr inline void setFlag(T _flag, bool _flagIsSet) {
			if (_flagIsSet) { m_data |= _flag; }
			else { m_data &= (~_flag); }
		};

		//! @brief Remove the provided flag
		//! @param _flag The flag to remove
		constexpr inline void removeFlag(T _flag) { m_data &= (~_flag); };

		//! @brief Returns true if the provided flag is set
		//! @param _flag the flag that should be checked
		constexpr inline bool flagIsSet(T _flag) const { return (m_data & _flag); };

		constexpr inline operator T(void) const { return m_data; };
		constexpr inline operator bool(void) const { return (bool)m_data; };

		constexpr inline Flags<T>& operator = (T _data) { m_data = _data; return *this; };
		constexpr inline Flags<T>& operator = (const Flags<T>& _other) { m_data = _other.m_data; return *this; };

		constexpr inline Flags<T> operator | (T _data) const { return Flags<T>{ m_data | _data }; };
		constexpr inline Flags<T> operator | (const Flags<T>& _other) const { return Flags<T>{ _other.m_data | m_data }; };

		constexpr inline Flags<T> operator & (T _data) const { return Flags<T>{ m_data & _data }; };
		constexpr inline Flags<T> operator & (const Flags<T>& _other) const { return Flags<T>{ _other.m_data & m_data }; };

		constexpr inline Flags<T>& operator |= (T _data) { m_data |= _data; return *this; };
		constexpr inline Flags<T>& operator |= (const Flags<T>& _other) { m_data |= _other.m_data; return *this; };

		constexpr inline Flags<T>& operator &= (T _data) { m_data &= _data; return *this; };
		constexpr inline Flags<T>& operator &= (const Flags<T>& _other) { m_data &= _other.m_data; return *this; };

		constexpr inline Flags<T> operator ~(void) { return Flags<T>{ ~m_data }; };

		constexpr inline bool operator == (T _data) const { return m_data == _data; };
		constexpr inline bool operator == (const Flags<T>& _other) const { return m_data == _other.m_data; };

		constexpr inline bool operator != (T _data) const { return m_data != _data; };
		constexpr inline bool operator != (const Flags<T>& _other) const { return m_data != _other.m_data; };
	};

}