//! @file Flags.h
//! @author Alexander Kuester (alexk95)
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTSystem/ArchitectureInfo.h"
#include "OTCore/CoreAPIExport.h"

// std header
#include <type_traits>

//! \brief Adds bitwise operations for the provided enum.
//! Use conveinience OT_ADD_FLAG_FUNCTIONS or OT_ADD_FRIEND_FLAG_FUNCTIONS.
#define OT_ADD_FLAG_FUNCTIONS_IMPL(___prefix, ___castType, ___enumName) ___prefix constexpr ___enumName operator | (___enumName _lhv, ___enumName _rhv) { return static_cast<___enumName>(static_cast<___castType>(_lhv) | static_cast<___castType>(_rhv)); }; \
___prefix constexpr ___enumName operator & (___enumName _lhv, ___enumName _rhv) { return static_cast<___enumName>(static_cast<___castType>(_lhv) & static_cast<___castType>(_rhv)); };  \
___prefix constexpr ___enumName operator ~ (___enumName _lhv) { return static_cast<___enumName>(~(static_cast<___castType>(_lhv))); };

#ifdef OT_OS_64Bit

//! \def OT_ADD_FLAG_FUNCTIONS
//! \brief Will add the default bitwise operations for the provided 64 bit bitfield.
//! Use this at the bottom of the File where the enum is defined.                 <br>
//!                                                                               <br>
//!		class MyClass {                                                           <br>
//!		public:                                                                   <br>
//!			enum MyEnum {                                                         <br>
//!				...                                                               <br>
//!			}                                                                     <br>
//!			typedef ot::Flags<MyEnum> MyFlags;                                    <br>
//!			// Not here                                                           <br>
//!		};                                                                        <br>
//!                                                                               <br>
//!		OT_ADD_FLAG_FUNCTIONS(MyClass::MyEnum)                                    <br>
//! 
//! \note Only enumration types are allowed.
#define OT_ADD_FLAG_FUNCTIONS(___enumName) OT_ADD_FLAG_FUNCTIONS_IMPL(, long long, ___enumName)

//! \def OT_ADD_FRIEND_FLAG_FUNCTIONS
//! \brief Will add the default bitwise operations for the provided private 64 bit bitfield.
//! Use this at the same scope inside the Class where the enum is defined.        <br>
//!                                                                               <br>
//!		class MyClass {                                                           <br>
//!		private:                                                                  <br>
//!			enum MyEnum {                                                         <br>
//!				...                                                               <br>
//!			}                                                                     <br>
//!			typedef ot::Flags<MyEnum> MyFlags;                                    <br>
//!			OT_ADD_FRIEND_FLAG_FUNCTIONS(MyClass::MyEnum)                         <br>
//!		};                                                                        <br>
//!                                                                               <br>
//!		// Not here                                                               <br>
//! 
//! \note Only enumration types are allowed.
#define OT_ADD_FRIEND_FLAG_FUNCTIONS(___enumName) OT_ADD_FLAG_FUNCTIONS_IMPL(friend , long long, ___enumName)

#else

//! \def OT_ADD_FLAG_FUNCTIONS
//! \brief Will add the default bitwise operations for the provided 32 bit bitfield.
//! Use this at the bottom of the File where the enum is defined.                 <br>
//!                                                                               <br>
//!		class MyClass {                                                           <br>
//!		public:                                                                   <br>
//!			enum MyEnum {                                                         <br>
//!				...                                                               <br>
//!			}                                                                     <br>
//!			typedef ot::Flags<MyEnum> MyFlags;                                    <br>
//!			// Not here                                                           <br>
//!		};                                                                        <br>
//!                                                                               <br>
//!		OT_ADD_FLAG_FUNCTIONS(MyClass::MyEnum)                                    <br>
//! 
//! \note Only enumration types are allowed.
#define OT_ADD_FLAG_FUNCTIONS(___enumName) OT_ADD_FLAG_FUNCTIONS_IMPL(, long, ___enumName)

//! \def OT_ADD_FRIEND_FLAG_FUNCTIONS
//! \brief Will add the default bitwise operations for the provided private 32 bit bitfield.
//! Use this at the same scope inside the Class where the enum is defined.        <br>
//!                                                                               <br>
//!		class MyClass {                                                           <br>
//!		private:                                                                  <br>
//!			enum MyEnum {                                                         <br>
//!				...                                                               <br>
//!			}                                                                     <br>
//!			typedef ot::Flags<MyEnum> MyFlags;                                    <br>
//!			OT_ADD_FRIEND_FLAG_FUNCTIONS(MyClass::MyEnum)                         <br>
//!		};                                                                        <br>
//!                                                                               <br>
//!		// Not here                                                               <br>
//! 
//! \note Only enumration types are allowed.
#define OT_ADD_FRIEND_FLAG_FUNCTIONS(___enumName) OT_ADD_FLAG_FUNCTIONS_IMPL(friend , long, ___enumName)

#endif

namespace ot {

	//! \class Flags
	//! \brief This class is used to manage flags.
	//! Don't forget to add OT_ADD_FLAG_FUNCTIONS and the bottom of your header.
	//! Use OT_ADD_FRIEND_FLAG_FUNCTIONS for private enums or public enums of private classes. <br>
	//! The type should be an enumeration where every value represents a single bit in a 32/64 bit value. <br>
	//! e.g: <br>
	//! enum enumName { <br>
	//!		enumValue1	= 0 << 0,	//	0000 <br>
	//!		enumValue2	= 1 << 0,	//	0001 <br>
	//!		enumValue3	= 1 << 1,	//	0010 <br>
	//!		enumValue4	= 1 << 2,	//	0100 <br>
	//!			... <br>
	//!	};
	//! \note Only enumration types are allowed.
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
		static_assert((std::is_enum<T>::value), "Flags accept only enumeration types.");

		T m_data; //! \brief Data.

	public:
		//! \brief Default constructor.
		//! Initializes the data with 0.
		inline constexpr Flags();

		//! \brief Assignment constructor.
		//! \param _initialData Initial data.
		inline constexpr Flags(T _initialData);

		//! \brief Copy constructor.
		//! \param _other Other flags.
		inline constexpr Flags(const Flags<T>& _other);

		//! \brief Move constructor;
		//! \param _other Other flags.
		inline constexpr Flags(Flags<T>&& _other) noexcept;

		//! @brief Returns a copy of the data
		inline constexpr T data(void) const;

		//! @brief Replace the current data
		//! @param _data The data that should be replaced with
		inline constexpr void set(T _data);

		//! @brief Set the provided flag
		//! @param _flag The flag to set
		inline constexpr void setFlag(T _flag);

		//! @brief Set or remove the provided flag
		//! @param _flag The flag to set
		//! @param _flagIsSet If true the flag will be set, otherwise removed
		inline constexpr void setFlag(T _flag, bool _flagIsSet);

		//! @brief Remove the provided flag
		//! @param _flag The flag to remove
		inline constexpr void removeFlag(T _flag);

		//! @brief Returns true if the provided flag is set
		//! @param _flag the flag that should be checked
		inline constexpr bool flagIsSet(T _flag) const;

		//! \brief Operator T is used to assign the Flags to a enum variable with the same type that is managed by Flags.
		inline constexpr operator T(void) const;

		//! \brief Returns true if at least one flag is set (assuming 0 = no flags).
		inline constexpr operator bool(void) const;

		inline constexpr Flags<T>& operator = (T _flag);
		inline constexpr Flags<T>& operator = (const Flags<T>& _other);

		//! @brief Move assign.
		//! @param _other Data will be copied to this object. Data in _other object will be set to 0.
		inline constexpr Flags<T>& operator = (Flags<T>&& _other) noexcept;

		inline constexpr Flags<T> operator | (T _flag) const;
		inline constexpr Flags<T> operator | (const Flags<T>& _other) const;

		inline constexpr Flags<T> operator & (T _flag) const;
		inline constexpr Flags<T> operator & (const Flags<T>& _other) const;

		inline constexpr Flags<T>& operator |= (T _flag);
		inline constexpr Flags<T>& operator |= (const Flags<T>& _other);

		inline constexpr Flags<T>& operator &= (T _flag);
		inline constexpr Flags<T>& operator &= (const Flags<T>& _other);

		inline constexpr Flags<T> operator ~(void) const;

		inline constexpr bool operator == (T _flag) const;
		inline constexpr bool operator == (const Flags<T>& _other) const;

		inline constexpr bool operator != (T _flag) const;
		inline constexpr bool operator != (const Flags<T>& _other) const;
	};
}

#include "OTCore/Flags.hpp"