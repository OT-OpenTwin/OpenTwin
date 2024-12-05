#pragma once

// OpenTwin header
#include "OTSystem/ArchitectureInfo.h"
#include "OTCore/CoreAPIExport.h"

// std header
#include <type_traits>

#ifdef OT_OS_64Bit

//! \def OT_ADD_FLAG_FUNCTIONS
//! \brief Will add the default bitwise operations for the provided 64 bit bitfield.
//! Use this at the bottom of the file where the enum and flags are defined.
//! 
//!		class MyClass {
//!		public:
//!			enum MyEnum {
//!				...
//!			}
//!			typedef ot::Flags<MyEnum> MyFlags;
//!		};
//! 
//!		OT_ADD_FLAG_FUNCTIONS(MyClass::MyEnum, MyClass::MyFlags)
//!		
//! \param ___enumName Full enum name (e.g. ot::EnumName).
//! \param ___flagsName Full flags name (e.g. ot::FlagsName).
#define OT_ADD_FLAG_FUNCTIONS(___enumName) constexpr ___enumName operator | (___enumName _lhv, ___enumName _rhv) { return static_cast<___enumName>(static_cast<long long>(_lhv) | static_cast<long long>(_rhv)); }; \
constexpr ___enumName operator & (___enumName _lhv, ___enumName _rhv) { return static_cast<___enumName>(static_cast<long long>(_lhv) & static_cast<long long>(_rhv)); };  \
constexpr ___enumName operator ~ (___enumName _lhv) { return static_cast<___enumName>(~(static_cast<long long>(_lhv))); };
#else
//! \def OT_ADD_FLAG_FUNCTIONS
//! \brief Will add the default bitwise operations for the provided 32 bit bitfield.
//! Use this at the bottom of the file where the enum and flags are defined.
//! 
//!		class MyClass {
//!		public:
//!			enum MyEnum {
//!				...
//!			}
//!			typedef ot::Flags<MyEnum> MyFlags;
//!		};
//! 
//!		OT_ADD_FLAG_FUNCTIONS(MyClass::MyEnum, MyClass::MyFlags)
//!		
//! \param ___enumName Full enum name (e.g. ot::EnumName).
//! \param ___flagsName Full flags name (e.g. ot::FlagsName).
#define OT_ADD_FLAG_FUNCTIONS(___enumName, ___flagsName) constexpr  ___enumName operator | (___enumName _lhv, ___enumName _rhv) { return static_cast<___enumName>(static_cast<long>(_lhv) | static_cast<long>(_rhv)); }; \
constexpr  ___enumName operator & (___enumName _lhv, ___enumName _rhv) { return static_cast<___enumName>(static_cast<long>(_lhv) & static_cast<long>(_rhv)); };  \
constexpr  ___enumName operator ~ (___enumName _lhv) { return static_cast<___enumName>(~(static_cast<long>(_lhv))); };
#endif

//! \def OT_FRIEND_FLAG_FUNCTIONS
//! \brief Adds a friend for the flag functions.
//! Use this at the bottom of the file where the enum and flags are defined.
//! 
//!		class MyClass {
//!		private:
//!			enum MyEnum {
//!				...
//!			}
//!			typedef ot::Flags<MyEnum> MyFlags;
//! 
//!			OT_FRIEND_FLAG_FUNCTIONS(MyClass::MyEnum, MyClass::MyFlags)
//!		};
//! 
//!		OT_ADD_FLAG_FUNCTIONS(MyClass::MyEnum, MyClass::MyFlags)
//!	
//! \warning If the class is located in a namespace add the OT_ADD_FLAG_FUNCTIONS macro inside of the namespace.
//! \param ___enumName Full enum name (e.g. ot::EnumName).
//! \param ___flagsName Full flags name (e.g. ot::FlagsName).
#define OT_FRIEND_FLAG_FUNCTIONS(___enumName) friend constexpr  ___enumName operator | (___enumName _lhv, ___enumName _rhv); \
friend constexpr  ___enumName operator & (___enumName _lhv, ___enumName _rhv);  \
friend constexpr  ___enumName operator ~ (___enumName _lhv);

#define OT_ADD_PRIVATE_FLAG_FUNCTIONS(___enumName) friend constexpr ___enumName operator | (___enumName _lhv, ___enumName _rhv) { return static_cast<___enumName>(static_cast<long long>(_lhv) | static_cast<long long>(_rhv)); }; \
friend constexpr ___enumName operator & (___enumName _lhv, ___enumName _rhv) { return static_cast<___enumName>(static_cast<long long>(_lhv) & static_cast<long long>(_rhv)); };  \
friend constexpr ___enumName operator ~ (___enumName _lhv) { return static_cast<___enumName>(~(static_cast<long long>(_lhv))); };

namespace ot {

	//! \class Flags
	//! \brief This class is used to manage flags.
	//! Don't forget to add OT_ADD_FLAG_FUNCTIONS and the bottom of your header.
	//! The type should be an enumeration where every value represents a single bit in a 32/64 bit value.
	//! e.g:
	//! enum enumName {
	//!		enumValue1	= 0x01,	//	0001
	//!		enumValue2	= 0x02,	//	0010
	//!		enumValue3	= 0x04,	//	0100
	//!			...
	//!	};
	//! \note Note that only enumration types are allowed.
	template<typename T> class OT_CORE_API_EXPORTONLY Flags {
	private:
#ifdef OT_OS_64Bit
		 //! \typedef FlagCastType
		 //! \brief Type used for casting the enum.
		typedef long long FlagCastType;
#else
		//! \typedef FlagCastType
		//! \brief Type used for casting the enum.
		typedef long FlagCastType;
#endif // _WIN64

		static_assert((sizeof(T) <= sizeof(FlagCastType)), "Flags type may overflow.");
		static_assert((std::is_enum<T>::value), "Flags accepts only enumeration types.");

		T m_data; //! \brief Data.

	public:
		//! \brief Default constructor.
		//! Initializes the data with 0.
		inline Flags() : m_data{ static_cast<T>(0) } {};

		//! \brief Assignment constructor.
		//! \param _initialData Initial data.
		inline Flags(T _initialData) : m_data{ _initialData } {}

		//! \brief Copy constructor.
		//! \param _other Other flags.
		inline Flags(const Flags<T>& _other) : m_data{ _other.data() } {};

		//! @brief Returns a copy of the data
		inline T data(void) const { return m_data; };

		//! @brief Replace the current data
		//! @param _data The data that should be replaced with
		inline void set(T _data) { m_data = _data; };

		//! @brief Set the provided flag
		//! @param _flag The flag to set
		inline void setFlag(T _flag) { m_data = m_data | _flag; };

		//! @brief Set or remove the provided flag
		//! @param _flag The flag to set
		//! @param _flagIsSet If true the flag will be set, otherwise removed
		inline void setFlag(T _flag, bool _flagIsSet) {
			if (_flagIsSet) { m_data = m_data | _flag; }
			else { m_data = m_data & (~_flag); }
		};

		//! @brief Remove the provided flag
		//! @param _flag The flag to remove
		inline void removeFlag(T _flag) { m_data = m_data & (~_flag); };

		//! @brief Returns true if the provided flag is set
		//! @param _flag the flag that should be checked
		inline bool flagIsSet(T _flag) const { return (m_data & _flag); };

		//! \brief Operator T is used to assign the Flags to a enum variable with the same type that is managed by Flags.
		inline operator T(void) const { return m_data; };

		//! \brief Returns true if at least one flag is set (assuming 0 = no flags).
		inline operator bool(void) const { return (bool)m_data; };

		inline Flags<T>& operator = (T _flag) { m_data = _flag; return *this; };
		inline Flags<T>& operator = (const Flags<T>& _other) { m_data = _other.m_data; return *this; };

		inline Flags<T> operator | (T _flag) const { return Flags<T>{ m_data | _flag }; };
		inline Flags<T> operator | (const Flags<T>& _other) const { return Flags<T>{ _other.m_data | m_data }; };

		inline Flags<T> operator & (T _flag) const { return Flags<T>{ m_data & _flag }; };
		inline Flags<T> operator & (const Flags<T>& _other) const { return Flags<T>{ _other.m_data & m_data }; };

		inline Flags<T>& operator |= (T _flag) { m_data = m_data | _flag; return *this; };
		inline Flags<T>& operator |= (const Flags<T>& _other) { m_data = m_data | _other.m_data; return *this; };

		inline Flags<T>& operator &= (T _flag) { m_data = m_data & _flag; return *this; };
		inline Flags<T>& operator &= (const Flags<T>& _other) { m_data = m_data & _other.m_data; return *this; };

		inline Flags<T> operator ~(void) const { return Flags<T>{ ~m_data }; };

		inline bool operator == (T _flag) const { return m_data == _flag; };
		inline bool operator == (const Flags<T>& _other) const { return m_data == _other.m_data; };

		inline bool operator != (T _flag) const { return m_data != _flag; };
		inline bool operator != (const Flags<T>& _other) const { return m_data != _other.m_data; };
	};

}