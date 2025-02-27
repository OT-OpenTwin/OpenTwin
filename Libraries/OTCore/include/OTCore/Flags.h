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
//! Use conveinience OT_ADD_FLAG_FUNCTIONS or OT_ADD_FRIEND_FLAG_FUNCTIONS for most cases.
#define OT_ADD_FLAG_FUNCTIONS_IMPL(___prefix, ___castType, ___enumName) ___prefix constexpr ___enumName operator | (___enumName _lhv, ___enumName _rhv) { return static_cast<___enumName>(static_cast<___castType>(_lhv) | static_cast<___castType>(_rhv)); }; \
___prefix constexpr ___enumName operator & (___enumName _lhv, ___enumName _rhv) { return static_cast<___enumName>(static_cast<___castType>(_lhv) & static_cast<___castType>(_rhv)); };  \
___prefix constexpr ___enumName operator ~ (___enumName _lhv) { return static_cast<___enumName>(~(static_cast<___castType>(_lhv))); };

#ifdef OT_OS_64Bit

//! \def OT_ADD_FLAG_FUNCTIONS
//! \brief Will add the default bitwise operations for the provided private 32/64 bit bitfield.
//! Use this if the enum is in a public scope.
//! 
//!		namespace MyNamespace {
//!			enum MyEnum {
//!				...
//!			};
//!			typedef ot::Flags<MyEnum> MyFlags;
//! 
//!			// or
//! 
//!			class MyClass {
//!			private:
//!				enum MyEnum2 {
//!					...
//!				};
//!				typedef ot::Flags<MyEnum2> MyFlags2;
//!			};
//!		}
//! 
//!		OT_ADD_FLAG_FUNCTIONS(MyNamespace::MyEnum)
//!		OT_ADD_FLAG_FUNCTIONS(MyClass::MyEnum2)
//! 
//! \note Only enumration types are allowed.
#define OT_ADD_FLAG_FUNCTIONS(___enumName) OT_ADD_FLAG_FUNCTIONS_IMPL(, long long, ___enumName)

//! \def OT_ADD_FRIEND_FLAG_FUNCTIONS
//! \brief Will add the default bitwise operations for the provided private 32/64 bit bitfield.
//! Use this at the same scope inside the class where the enum is defined. <br>
//! It is not wrong to use OT_ADD_FRIEND_FLAG_FUNCTIONS instead of OT_ADD_FLAG_FUNCTIONS but the scope is important.
//! 
//!		class MyClass {
//!		private:
//!			enum MyEnum {
//!				...
//!			};
//!			typedef ot::Flags<MyEnum> MyFlags;
//! 
//!			// Friend is needed since enum is in a private scope.
//!			OT_ADD_FRIEND_FLAG_FUNCTIONS(MyClass::MyEnum)
//!		};
//! 
//!		//  - - - or - - -
//! 
//!		class MyClass2 {
//!		public:
//!			enum MyEnum2 {
//!				...
//!			};
//!			typedef ot::Flags<MyEnum2> MyFlags2;
//! 
//!			// Friend is not needed but also not wrong.
//!			// Instead of OT_ADD_FRIEND_FLAG_FUNCTIONS use:
//!			// OT_ADD_FLAG_FUNCTIONS at the end of the file.
//!			OT_ADD_FRIEND_FLAG_FUNCTIONS(MyClass2::MyEnum2)
//!		};
//! 
//!		//  - - - or - - -
//! 
//!		class MyClass3 {
//!		private:
//!				class MyNestedClass {
//!				public:
//!					enum MyEnum3 {
//!						...
//!					};
//!					typedef ot::Flags<MyEnum3> MyFlags3;
//! 
//!					// Friend is needed since it is a public enum
//!					// but the parent class is in a private scope.
//!					OT_ADD_FRIEND_FLAG_FUNCTIONS(MyClass3::MyEnum3)
//!				};
//!		};
//! 
//!		// < Here only OT_ADD_FLAG_FUNCTIONS >
//! 
//! \note Only enumration types are allowed.
#define OT_ADD_FRIEND_FLAG_FUNCTIONS(___enumName) OT_ADD_FLAG_FUNCTIONS_IMPL(friend , long long, ___enumName)

#else

//! \def OT_ADD_FLAG_FUNCTIONS
//! \brief Will add the default bitwise operations for the provided private 32/64 bit bitfield.
//! Use this if the enum is in a public scope.
//! 
//!		namespace MyNamespace {
//!			enum MyEnum {
//!				...
//!			};
//!			typedef ot::Flags<MyEnum> MyFlags;
//! 
//!			// or
//! 
//!			class MyClass {
//!			private:
//!				enum MyEnum2 {
//!					...
//!				};
//!				typedef ot::Flags<MyEnum2> MyFlags2;
//!			};
//!		}
//! 
//!		OT_ADD_FLAG_FUNCTIONS(MyNamespace::MyEnum)
//!		OT_ADD_FLAG_FUNCTIONS(MyClass::MyEnum2)
//! 
//! \note Only enumration types are allowed.
#define OT_ADD_FLAG_FUNCTIONS(___enumName) OT_ADD_FLAG_FUNCTIONS_IMPL(, long, ___enumName)

//! \def OT_ADD_FRIEND_FLAG_FUNCTIONS
//! \brief Will add the default bitwise operations for the provided private 32/64 bit bitfield.
//! Use this at the same scope inside the class where the enum is defined. <br>
//! It is not wrong to use OT_ADD_FRIEND_FLAG_FUNCTIONS instead of OT_ADD_FLAG_FUNCTIONS but the scope is important.
//! 
//!		class MyClass {
//!		private:
//!			enum MyEnum {
//!				...
//!			};
//!			typedef ot::Flags<MyEnum> MyFlags;
//! 
//!			// Friend is needed since enum is in a private scope.
//!			OT_ADD_FRIEND_FLAG_FUNCTIONS(MyClass::MyEnum)
//!		};
//! 
//!		//  - - - or - - -
//! 
//!		class MyClass2 {
//!		public:
//!			enum MyEnum2 {
//!				...
//!			};
//!			typedef ot::Flags<MyEnum2> MyFlags2;
//! 
//!			// Friend is not needed but also not wrong.
//!			// Instead of OT_ADD_FRIEND_FLAG_FUNCTIONS use:
//!			// OT_ADD_FLAG_FUNCTIONS at the end of the file.
//!			OT_ADD_FRIEND_FLAG_FUNCTIONS(MyClass2::MyEnum2)
//!		};
//! 
//!		//  - - - or - - -
//! 
//!		class MyClass3 {
//!		private:
//!				class MyNestedClass {
//!				public:
//!					enum MyEnum3 {
//!						...
//!					};
//!					typedef ot::Flags<MyEnum3> MyFlags3;
//! 
//!					// Friend is needed since it is a public enum
//!					// but the parent class is in a private scope.
//!					OT_ADD_FRIEND_FLAG_FUNCTIONS(MyClass3::MyEnum3)
//!				};
//!		};
//! 
//!		// < Here only OT_ADD_FLAG_FUNCTIONS >
//! 
//! \note Only enumration types are allowed.
#define OT_ADD_FRIEND_FLAG_FUNCTIONS(___enumName) OT_ADD_FLAG_FUNCTIONS_IMPL(friend , long, ___enumName)

#endif

namespace ot {

	//! \class Flags
	//! \brief The Flags class is a wrapper around a enum that allows bitwise operations (flags).
	//! OT_ADD_FLAG_FUNCTIONS or custom bitwise operations must be provided for the enum.
	//! The type should be an enumeration where every value represents a single bit in a 32/64 bit value.
	//! 
	//!		// This enum contains values represeting single bits,
	//!		// or masks used for bitwise operations.
	//!		enum EnumName {
	//!			// This should always be provided as somewhat of default value,
	//!			// since ot::Flags<EnumName> will assume 0x00 to be the default.
	//!			//                                   // +------+------+---------+
	//!			//                                   // | Hex  | Bin  |  Info   |
	//!			//                                   // +------+------+---------+
	//!			EmptyFlags = 0 << 0,                 // | 0x00 | 0000 | Default |
	//!			EnumValue1 = 1 << 0,                 // | 0x01 | 0001 | Value 1 |
	//!			EnumValue2 = 1 << 1,	             // | 0x02 | 0010 | Value 2 |
	//!			EnumMask1  = EnumValue1 | EnumValue2 // | 0x03 | 0011 | Mask 1  |
	//!			EnumValue3 = 1 << 2,                 // | 0x04 | 0100 | Value 3 |
	//!			...                                  // +------+------+---------+
	//!		};
	//! 
	//! \note Only enumration types are allowed.
	//! The enumerations should provided const expressions in the same (or higher) scope the flags are used at.
	//! 
	//! \ref OT_ADD_FLAG_FUNCTIONS
	//! \ref OT_ADD_FRIEND_FLAG_FUNCTIONS
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
		inline Flags() : Flags(0) {};

		//! \brief Assignment constructor.
		//! \param _initialData Initial data.
		inline Flags(T _initialData) : m_data{ _initialData } {};

		//! \brief Assignment constructor.
		//! \param _initialData Initial data.
		explicit inline Flags(FlagCastType _initialData) : m_data{ static_cast<T>(_initialData) } {};

		//! \brief Copy constructor.
		//! \param _other Other flags.
		inline Flags(const Flags<T>& _other) : m_data{ _other.data() } {};
		
		//! \brief Move constructor.
		//! \param _other Other flags.
		inline Flags(Flags&& _other) noexcept : m_data(std::move(_other.m_data)) {};

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
		inline Flags<T>& operator = (Flags<T>&& _other) { m_data = std::move(_other.m_data); return *this; };

		inline Flags<T> operator | (T _flag) const { return Flags<T>{ m_data | _flag }; };
		inline Flags<T> operator | (const Flags<T>& _other) const { return Flags<T>{ _other.m_data | m_data }; };

		inline Flags<T> operator & (T _flag) const { return Flags<T>{ m_data& _flag }; };
		inline Flags<T> operator & (const Flags<T>& _other) const { return Flags<T>{ _other.m_data& m_data }; };

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