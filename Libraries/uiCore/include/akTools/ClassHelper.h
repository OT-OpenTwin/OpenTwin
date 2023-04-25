#pragma once

//! @brief Creates a private property with public set and get function
//! The access specifier after the macro will be "public" (See example)
//! Example:
//!		Code:
//!			A_PROPERTY(int, Test);
//!
//!		Result:
//!			private:
//!				int m_Test;
//!			public:
//!				void setTest(int _Test) { m_Test = _Test; };
//!				int Test(void) const { return m_Test; };
//!
//! @param ___type The property type
//! @param ___name The property name
#define A_PROPERTY(___type, ___name) private: ___type m_##___name; public: void set##___name(const ___type& _##___name) { m_##___name = _##___name; }; ___type ___name(void) const { return m_##___name; };

//! @brief Creates a private property with public set and get function
//! The difference to A_PROPERTY is that a pointer type is expected
//! The access specifier after the macro will be "public" (See example)
//! Example:
//!		Code:
//!			A_PROPERTY(int, Test);
//!
//!		Result:
//!			private:
//!				int m_Test;
//!			public:
//!				void setTest(int _Test) { m_Test = _Test; };
//!				int Test(void) const { return m_Test; };
//!
//! @param ___type The property type
//! @param ___name The property name
#define A_PROPERTY_POINTER(___type, ___name) private: ___type m_##___name; public: void set##___name(___type _##___name) { m_##___name = _##___name; }; ___type ___name(void) const { return m_##___name; };

//! @brief Creates a private property with public set and get function
//! The difference to A_PROPERTY is that a pointer type is expected
//! The difference to A_PROPERTY_POINTER is that the get function will start with the keyword "get"
//! The access specifier after the macro will be "public" (See example)
//! Example:
//!		Code:
//!			A_PROPERTY(int, Test);
//!
//!		Result:
//!			private:
//!				int m_Test;
//!			public:
//!				void setTest(int _Test) { m_Test = _Test; };
//!				int Test(void) const { return m_Test; };
//!
//! @param ___type The property type
//! @param ___name The property name
#define A_PROPERTY_POINTERGET(___type, ___name) private: ___type m_##___name; public: void set##___name(___type _##___name) { m_##___name = _##___name; }; ___type get##___name(void) const { return m_##___name; };

//! @brief Creates a private property with public set and get function
//! The difference to A_PROPERTY is that the get function will start with the keyword "get"
//! The access specifier after the macro will be "public" (See example)
//! Example:
//!		Code:
//!			A_PROPERTY(int, Test);
//!
//!		Result:
//!			private:
//!				int m_Test;
//!			public:
//!				void setTest(int _Test) { m_Test = _Test; };
//!				int getTest(void) const { return m_Test; };
//!
//! @param ___type The property type
//! @param ___name The property name
#define A_PROPERTY_GET(___type, ___name) private: ___type m_##___name; public: void set##___name(const ___type& _##___name) { m_##___name = _##___name; }; ___type get##___name(void) const { return m_##___name; };

//! @brief Creates a private property with public set and get function
//! The difference to A_PROPERTY is that the get function will return a reference to the member
//! The access specifier after the macro will be "public" (See example)
//! Example:
//!		Code:
//!			A_PROPERTY(int, Test);
//!
//!		Result:
//!			private:
//!				int m_Test;
//!			public:
//!				void setTest(int _Test) { m_Test = _Test; };
//!				int& Test(void) { return m_Test; };
//!
//! @param ___type The property type
//! @param ___name The property name
#define A_PROPERTY_REF(___type, ___name) private: ___type m_##___name; public: void set##___name(const ___type& _##___name) { m_##___name = _##___name; }; ___type& ___name(void) { return m_##___name; };

//! @brief Creates a private property with public set and get function
//! The difference to A_PROPERTY is that the get function will return a const reference to the member
//! The access specifier after the macro will be "public" (See example)
//! Example:
//!		Code:
//!			A_PROPERTY(int, Test);
//!
//!		Result:
//!			private:
//!				int m_Test;
//!			public:
//!				void setTest(int _Test) { m_Test = _Test; };
//!				const int& Test(void) const { return m_Test; };
//!
//! @param ___type The property type
//! @param ___name The property name
#define A_PROPERTY_CREF(___type, ___name) private: ___type m_##___name; public: void set##___name(const ___type& _##___name) { m_##___name = _##___name; }; const ___type& ___name(void) const { return m_##___name; };

//! @brief Creates a private property with public set and get function
//! The difference to A_PROPERTY is that the get function will return a reference to the member
//! The difference to A_PROPERTY_REF is that the get function will start with the keyword "get"
//! The access specifier after the macro will be "public" (See example)
//! Example:
//!		Code:
//!			A_PROPERTY(int, Test);
//!
//!		Result:
//!			private:
//!				int m_Test;
//!			public:
//!				void setTest(int _Test) { m_Test = _Test; };
//!				int& getTest(void) { return m_Test; };
//!
//! @param ___type The property type
//! @param ___name The property name
#define A_PROPERTY_REFGET(___type, ___name) private: ___type m_##___name; public: void set##___name(const ___type& _##___name) { m_##___name = _##___name; }; ___type& get##___name(void) { return m_##___name; };

//! @brief Creates a private property with public set and get function
//! The difference to A_PROPERTY is that the get function will return a const reference to the member
//! The difference to A_PROPERTY_CREF is that the get function will start with the keyword "get"
//! The access specifier after the macro will be "public" (See example)
//! Example:
//!		Code:
//!			A_PROPERTY(int, Test);
//!
//!		Result:
//!			private:
//!				int m_Test;
//!			public:
//!				void setTest(int _Test) { m_Test = _Test; };
//!				const int& getTest(void) const { return m_Test; };
//!
//! @param ___type The property type
//! @param ___name The property name
#define A_PROPERTY_CREFGET(___type, ___name) private: ___type m_##___name; public: void set##___name(const ___type& _##___name) { m_##___name = _##___name; }; const ___type& get##___name(void) const { return m_##___name; };

//! @brief Creates a private property with public set and get function
//! The type of the member will be bool
//! The get function will start with the keyword "is"
//! The access specifier after the macro will be "public"
//! @param ___name The property name
#define A_PROPERTY_IS(___name) private: bool m_##___name; public: void set##___name(bool _##___name) { m_##___name = _##___name; }; bool is##___name(void) const { return m_##___name; };

//! @brief Creates a private property with public set and get function
//! The type of the member will be bool
//! The get function will start with the keyword "has"
//! The access specifier after the macro will be "public"
//! @param ___type The property type
//! @param ___name The property name
#define A_PROPERTY_HAS(___name) private: bool m_##___name; public: void set##___name(bool _##___name) { m_##___name = _##___name; }; bool has##___name(void) const { return m_##___name; };

//! @brief Creates a protected property with public set and get function
//! The access specifier after the macro will be "public"
//! @param ___type The property type
//! @param ___name The property name
#define A_PROPERTY_P(___type, ___name) protected: ___type m_##___name; public: void set##___name(const ___type& _##___name) { m_##___name = _##___name; }; const ___type& ___name(void) const { return m_##___name; };