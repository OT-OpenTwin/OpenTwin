//! @file Property.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/Property.h"

ot::Property::Property(PropertyFlags _flags) 
	: m_flags(_flags)
{}

ot::Property::Property(const std::string& _name, PropertyFlags _flags)
	: m_name(_name), m_flags(_flags)
{}

std::string ot::Property::toString(PropertyType _type) {
	switch (_type)
	{
	case ot::Property::BoolType: return "Bool";
	case ot::Property::IntType: return "Int";
	case ot::Property::DoubleType: return "Double";
	case ot::Property::StringType: return "String";
	case ot::Property::StringListType: return "StringList";
	case ot::Property::ColorType: return "Color";
	case ot::Property::DirectoryType: return "Directory";
	case ot::Property::FilePathType: return "FilePath";
	case ot::Property::Painter2DType: return "Painter2DType";
	default:
		OT_LOG_EA("Unknown Property Type");
		return "<null>";
	}
}

ot::Property::PropertyType ot::Property::stringToPropertyType(const std::string& _type) {
	if (_type == Property::toString(BoolType)) return BoolType;
	else if (_type == Property::toString(IntType)) return IntType;
	else if (_type == Property::toString(DoubleType)) return DoubleType;
	else if (_type == Property::toString(StringType)) return StringType;
	else if (_type == Property::toString(StringListType)) return StringListType;
	else if (_type == Property::toString(ColorType)) return ColorType;
	else if (_type == Property::toString(DirectoryType)) return DirectoryType;
	else if (_type == Property::toString(FilePathType)) return FilePathType;
	else if (_type == Property::toString(Painter2DType)) return Painter2DType;
	else {
		OT_LOG_EA("Unknown Property Type");
		return BoolType;
	}
}

std::string ot::Property::toString(PropertyFlags _flag) {
	switch (_flag)
	{
	case ot::Property::NoFlags: return "<null>";
	case ot::Property::IsReadOnly: return "IsReadOnly";
	case ot::Property::IsProtected: return "IsProtected";
	case ot::Property::IsHidden: return "IsHidden";
	case ot::Property::HasInputError: return "HasInputError";
	case ot::Property::HasMultipleValues: return "HasMultipleValues";
	case ot::Property::IsDeletable: return "IsDeletable";
	case ot::Property::AllowCustomValues: return "AllowCustomValues";
	case ot::Property::AllowMultiselection: return "AllowMultiselection";
	default:
		OT_LOG_EA("Unknown Property Flag");
		return "<null>";
	}
}

ot::Property::PropertyFlags ot::Property::stringToFlag(const std::string& _flag) {
	if (_flag == Property::toString(Property::NoFlags)) { return Property::NoFlags; }
	else if (_flag == Property::toString(Property::IsReadOnly)) { return Property::IsReadOnly; }
	else if (_flag == Property::toString(Property::IsProtected)) { return Property::IsProtected; }
	else if (_flag == Property::toString(Property::IsHidden)) { return Property::IsHidden; }
	else if (_flag == Property::toString(Property::HasInputError)) { return Property::HasInputError; }
	else if (_flag == Property::toString(Property::HasMultipleValues)) { return Property::HasMultipleValues; }
	else if (_flag == Property::toString(Property::IsDeletable)) { return Property::IsDeletable; }
	else if (_flag == Property::toString(Property::AllowCustomValues)) { return Property::AllowCustomValues; }
	else if (_flag == Property::toString(Property::AllowMultiselection)) { return Property::AllowMultiselection; }
	else {
		OT_LOG_EAS("Unknown Property Flag \"" + _flag + "\"");
		return Property::NoFlags;
	}
}

std::list<std::string> ot::Property::toStringList(PropertyFlags _flags) {
	std::list<std::string> ret;
	if (_flags & Property::IsReadOnly) ret.push_back(Property::toString(Property::IsReadOnly));
	if (_flags & Property::IsHidden) ret.push_back(Property::toString(Property::IsHidden));
	if (_flags & Property::IsProtected) ret.push_back(Property::toString(Property::IsProtected));
	if (_flags & Property::HasInputError) ret.push_back(Property::toString(Property::HasInputError));
	if (_flags & Property::HasMultipleValues) ret.push_back(Property::toString(Property::HasMultipleValues));
	if (_flags & Property::IsDeletable) ret.push_back(Property::toString(Property::IsDeletable));
	if (_flags & Property::AllowCustomValues) ret.push_back(Property::toString(Property::AllowCustomValues));
	if (_flags & Property::AllowMultiselection) ret.push_back(Property::toString(Property::AllowMultiselection));
	return ret;
}

ot::Property::PropertyFlags ot::Property::stringListToFlags(const std::list<std::string>& _flags) {
	Property::PropertyFlags ret = Property::NoFlags;
	for (const std::string& f : _flags) {
		ret |= Property::stringToFlag(f);
	};
	return ret;
}

void ot::Property::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember(OT_JSON_MEMBER_Property_Type, JsonString(this->toString(this->getPropertyType()), _allocator), _allocator);
	_object.AddMember("Tip", JsonString(m_tip, _allocator), _allocator);
	_object.AddMember("Name", JsonString(m_name, _allocator), _allocator);
	_object.AddMember("Title", JsonString(m_title, _allocator), _allocator);
	_object.AddMember("SpecialType", JsonString(m_specialType, _allocator), _allocator);
	_object.AddMember("Data", JsonString(m_data, _allocator), _allocator);
	_object.AddMember("Flags", JsonArray(this->toStringList(m_flags), _allocator), _allocator);
	this->getPropertyData(_object, _allocator);
}

void ot::Property::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_tip = json::getString(_object, "Tip");
	m_name = json::getString(_object, "Name");
	m_title = json::getString(_object, "Title");
	m_specialType = json::getString(_object, "SpecialType");
	m_data = json::getString(_object, "Data");
	m_flags = this->stringListToFlags(json::getStringList(_object, "Flags"));

	this->setPropertyData(_object); 
}

void ot::Property::setFromOther(const Property* _other) {
	m_tip = _other->m_tip;
	m_name = _other->m_name;
	m_title = _other->m_title;
	m_specialType = _other->m_specialType;
	m_data = _other->m_data;
	m_flags = _other->m_flags;
}
