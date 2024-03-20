#pragma once

#define OT_PROPERTYBASE_MEMBERNAME(___name) m_##___name
#define OT_PROPERTYBASE_MEMBER(___type, ___name) ___type OT_PROPERTYBASE_MEMBERNAME(___name)
#define OT_PROPERTYBASE_SET(___type, ___name, ___setter) void ___setter(___type _v) { OT_PROPERTYBASE_MEMBERNAME(___name) = _v; }
#define OT_PROPERTYBASE_SET_REF(___type, ___name, ___setter) void ___setter(const ___type& _v) { OT_PROPERTYBASE_MEMBERNAME(___name) = _v; }
#define OT_PROPERTYBASE_GET(___type, ___name, ___getter) ___type ___getter(void) const { return OT_PROPERTYBASE_MEMBERNAME(___name); }
#define OT_PROPERTYBASE_GET_REF(___type, ___name, ___getter) const ___type& ___getter(void) const { return OT_PROPERTYBASE_MEMBERNAME(___name); }

//! @brief Creates a private property with public set and get functions
//! @param ___type Property type
//! @param ___name Property name
//! @param ___setter Setter function name
//! @param ___getter Getter function name
#define OT_PROPERTY_V(___type, ___name, ___setter, ___getter, ___scope) private: OT_PROPERTYBASE_MEMBER(___type, ___name); ___scope: OT_PROPERTYBASE_SET(___type, ___name, ___setter); OT_PROPERTYBASE_GET(___type, ___name, ___getter);

//! @brief Creates a private property with public set and get functions
//! @param ___type Property type
//! @param ___name Property name
//! @param ___setter Setter function name
//! @param ___getter Getter function name
#define OT_PROPERTY(___type, ___name, ___setter, ___getter) OT_PROPERTY_V(___type, ___name, ___setter, ___getter, public)

//! @brief Creates a private property with public set and get functions which will receive or return const references
//! @param ___type Property type
//! @param ___name Property name
//! @param ___setter Setter function name
//! @param ___getter Getter function name
#define OT_PROPERTY_REF_V(___type, ___name, ___setter, ___getter, ___scope) private: OT_PROPERTYBASE_MEMBER(___type, ___name); ___scope: OT_PROPERTYBASE_SET_REF(___type, ___name, ___setter); OT_PROPERTYBASE_GET_REF(___type, ___name, ___getter);

//! @brief Creates a private property with public set and get functions which will receive or return const references
//! @param ___type Property type
//! @param ___name Property name
//! @param ___setter Setter function name
//! @param ___getter Getter function name
#define OT_PROPERTY_REF(___type, ___name, ___setter, ___getter) OT_PROPERTY_REF_V(___type, ___name, ___setter, ___getter, public)

//! @brief Creates a private property with public set and get functions
//! @param ___type Property type
//! @param ___name Property name
//! @param ___getter Getter function name
#define OT_PROPERTY_NOSET(___type, ___name, ___getter) private: OT_PROPERTYBASE_MEMBER(___type___name); public: OT_PROPERTYBASE_GET(___type, ___name, ___getter);

//! @brief Removes the default copy constructor and assignment operator
#define OT_DECL_NOCOPY(___class) ___class(const ___class&) = delete; ___class& operator = (const ___class&) = delete;

//! @brief Removes the default copy constructor and assignment operator
#define OT_DECL_NODEFAULT(___class) ___class(void) = delete;