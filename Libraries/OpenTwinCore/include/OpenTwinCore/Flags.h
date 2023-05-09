#pragma once

// OpenTwin header
#include "OpenTwinSystem/ArchitectureInfo.h"

#ifdef OT_OS_64Bit
//! @brief Will add the default bit operations thats are required for bitwise operations
#define OT_ADD_FLAG_FUNCTIONS(___enumName) inline ___enumName operator | (___enumName _lhv, ___enumName _rhv) { return (___enumName)((long long) _lhv | (long long)_rhv); }; \
inline ___enumName & operator |= (___enumName & _lhv, ___enumName _rhv) { return (_lhv = (___enumName)((long long) _lhv | (long long)_rhv)); }; \
inline ___enumName operator & (___enumName _lhv, ___enumName _rhv) { return (___enumName)((long long) _lhv & (long long)_rhv); };  \
inline ___enumName & operator &= (___enumName & _lhv, ___enumName _rhv) { return (_lhv = (___enumName)((long long) _lhv & (long long)_rhv)); };  \
inline ___enumName operator ~ (___enumName _lhv) { return (___enumName)(~((long long) _lhv)); };
#else
//! @brief Will add the default bit operations thats are required for bitwise operations
#define OT_ADD_FLAG_FUNCTIONS(___enumName) inline ___enumName operator | (___enumName _lhv, ___enumName _rhv) { return (___enumName)((long) _lhv | (long)_rhv); }; \
inline ___enumName & operator |= (___enumName & _lhv, ___enumName _rhv) { return (_lhv = (___enumName)((long) _lhv | (long)_rhv)); }; \
inline ___enumName operator & (___enumName _lhv, ___enumName _rhv) { return (___enumName)((long) _lhv & (long)_rhv); };  \
inline ___enumName & operator &= (___enumName & _lhv, ___enumName _rhv) { return (_lhv = (___enumName)((long) _lhv & (long)_rhv)); };  \
inline ___enumName operator ~ (___enumName _lhv) { return (___enumName)(~((long) _lhv)); };
#endif

namespace ot {

	//! This class is used to manage flags
	//! The type should be an enum where every value represents a single bit in 32/64 bit value
	//! e.g:
	//! enum enumName {
	//!		enumValue1	= 0x01,	//	0001
	//!		enumValue2	= 0x02,	//	0010
	//!		enumValue3	= 0x04,	//	0100
	//!			...
	//!	}
	template<class T> class Flags {
	private:
#ifdef OT_OS_64Bit
		typedef long long FlagCastType;
#else
		typedef long FlagCastType;
#endif // _WIN64

		T m_data;

	public:
		Flags() : m_data{ static_cast<T>(0) } {}
		Flags(const Flags<T> & _other) : m_data{ _other.data() } {}
		Flags(T _initialData) : m_data{ _initialData } {}

		virtual ~Flags() {}

		//! @brief Returns a copy of the data
		inline T data(void) const { return m_data; }

		//! @brief Replace data if data differs
		//! @param _other The other flags
		//! @return True if the data was updated
		inline bool update(const Flags<T>& _other) { if (*this == _other) { return false; } else { m_data = _other.m_data; return true; } };

		//! @brief Replace the current data
		//! @param _data The data that should be replaced with
		inline void replaceWith(T _data) { m_data = _data; }

		//! @brief Set the provided flag
		//! @param _flag The flag to set
		inline void setFlag(T _flag) { m_data = static_cast<T>(static_cast<FlagCastType>(m_data) | static_cast<FlagCastType>(_flag)); }

		//! @brief Set or remove the provided flag
		//! @param _flag The flag to set
		//! @param _flagIsSet If true the flag will be set, otherwise removed
		inline void setFlag(T _flag, bool _flagIsSet) {
			if (_flagIsSet) { m_data = static_cast<T>(static_cast<FlagCastType>(m_data) | static_cast<FlagCastType>(_flag)); }
			else { m_data = static_cast<T>(static_cast<FlagCastType>(m_data) & (~static_cast<FlagCastType>(_flag))); }
		}

		//! @brief Remove the provided flag
		//! @param _flag The flag to remove
		inline void removeFlag(T _flag) { m_data = static_cast<T>(static_cast<FlagCastType>(m_data) & (~static_cast<FlagCastType>(_flag))); }

		//! @brief Returns true if the provided flag is set
		//! @param _flag the flag that should be checked
		inline bool flagIsSet(T _flag) const { return (static_cast<FlagCastType>(m_data) & static_cast<FlagCastType>(_flag)) == static_cast<FlagCastType>(_flag); }

		inline Flags<T>& operator = (const Flags<T> & _other) { m_data = _other.data(); return *this; }
		inline Flags<T> operator | (const Flags<T> & _other) const { return Flags<T>{ static_cast<T>(static_cast<FlagCastType>(_other.data()) | static_cast<FlagCastType>(m_data)) }; }
		inline Flags<T> operator & (const Flags<T> & _other) const { return Flags<T>{ static_cast<T>(static_cast<FlagCastType>(_other.data()) & static_cast<FlagCastType>(m_data)) }; }
		inline Flags<T>& operator |= (const Flags<T> & _other) { m_data = static_cast<T>(static_cast<FlagCastType>(_other.data()) | static_cast<FlagCastType>(m_data)); return *this; }
		inline Flags<T>& operator &= (const Flags<T> & _other) { m_data = static_cast<T>(static_cast<FlagCastType>(_other.data()) & static_cast<FlagCastType>(m_data)); return *this; }
		inline Flags<T>& operator -= (const Flags<T> & _other) { m_data = static_cast<T>(static_cast<FlagCastType>(_other.data()) & ~static_cast<FlagCastType>(m_data)); return *this; }
		inline Flags<T> operator ~(void) { return Flags<T>{ static_cast<T>(~static_cast<FlagCastType>(m_data)) }; }
		inline bool operator == (const Flags<T> & _other) const { return static_cast<FlagCastType>(m_data) == static_cast<FlagCastType>(_other.data()); }
		inline bool operator != (const Flags<T> & _other) const { return static_cast<FlagCastType>(m_data) != static_cast<FlagCastType>(_other.data()); }
	};

}
