#pragma once

//! @brief Creates a private property with public set and get function
//! @param ___type The property type
//! @param ___name The property name
#define OT_PROPERTY(___type, ___name) private: ___type m_##___name; public: void set##___name(const ___type& _##___name) { m_##___name = _##___name; }; const ___type& ___name(void) const { return m_##___name; };

//! @brief Creates a private property with public set and get function
//! The difference to OT_PROPERTY is that the getter function will be initiated with "get"
//! @param ___type The property type
//! @param ___name The property name
#define OT_PROPERTY_GET(___type, ___name) private: ___type m_##___name; public: void set##___name(const ___type& _##___name) { m_##___name = _##___name; }; const ___type& get##___name(void) const { return m_##___name; };

//! @brief Creates a protected property with public set and get function
//! The difference to OT_PROPERTY is that the getter function will be initiated with "is" and the member type will be bool
//! @param ___type The property type
//! @param ___name The property name
#define OT_PROPERTY_IS(___name) protected: bool m_##___name; public: void set##___name(bool _##___name) { m_##___name = _##___name; }; bool is##___name(void) const { return m_##___name; };

//! @brief Creates a protected property with public set and get function
//! The difference to OT_PROPERTY is that the getter function will be initiated with "has" and the member type will be bool
//! @param ___type The property type
//! @param ___name The property name
#define OT_PROPERTY_HAS(___name) protected: bool m_##___name; public: void set##___name(bool _##___name) { m_##___name = _##___name; }; bool has##___name(void) const { return m_##___name; };

//! @brief Creates a protected property with public set and get function
//! @param ___type The property type
//! @param ___name The property name
#define OT_PROPERTY_P(___type, ___name) protected: ___type m_##___name; public: void set##___name(const ___type& _##___name) { m_##___name = _##___name; }; const ___type& ___name(void) const { return m_##___name; };

//! @brief Creates a property with the current visibility (if declared in private then the set and get will be private)
//! @param ___type The property type
//! @param ___name The property name
#define OT_PROPERTY_NV(___type, ___name) ___type m_##___name; void set##___name(const ___type& _##___name) { m_##___name = _##___name; }; const ___type& ___name(void) const { return m_##___name; };



#define OT_DECL_NOCOPY(___class) ___class(const ___class&) = delete; ___class(const ___class&&) = delete; ___class& operator = (const ___class&) = delete;