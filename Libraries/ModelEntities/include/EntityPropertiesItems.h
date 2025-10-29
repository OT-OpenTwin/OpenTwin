// @otlicense

//! @file EntityPropertiesItems.h
//! @author Peter Thoma, Alexander Kuester (alexk95)
//! @date February 2020
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once
#pragma warning(disable : 4251)

// OpenTwin header
#include "OTGui/PropertyGridCfg.h"
#include "OTGui/Painter2DDialogFilter.h"
#include "OldTreeIcon.h"
#include "ModelEntitiesAPIExport.h"

// std header
#include <string>
#include <vector>

class EntityBase;
class EntityContainer;
class EntityProperties;

namespace ot { class Property; };
namespace ot { class Painter2D; };

class OT_MODELENTITIES_API_EXPORT EntityPropertiesBase
{
public:
	EntityPropertiesBase();
	virtual ~EntityPropertiesBase() {};

	EntityPropertiesBase(const EntityPropertiesBase &other);

	virtual EntityPropertiesBase *createCopy() const = 0;

	void setContainer(EntityProperties *c) { m_container = c; };

	void setName(const std::string& _name) { m_name = _name; };
	const std::string& getName() const { return m_name; };

	void setGroup(const std::string& _group) { m_group = _group; };
	const std::string& getGroup() const { return m_group; };

	enum eType { DOUBLE, INTEGER, BOOLEAN, STRING, SELECTION, COLOR, ENTITYLIST, PROJECTLIST, GUIPAINTER };
	virtual eType getType() const = 0;
	virtual std::string getTypeString() const = 0;

	void resetNeedsUpdate() { m_needsUpdateFlag = false; };
	bool needsUpdate();
	void setNeedsUpdate();

	void setHasMultipleValues(bool b) { m_multipleValues = b; };
	bool hasMultipleValues() const { return m_multipleValues; };

	virtual bool isCompatible(EntityPropertiesBase *other) const { return true; };
	virtual bool hasSameValue(EntityPropertiesBase *other) const = 0;

	virtual void addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase *root) = 0;
	virtual void setFromConfiguration(const ot::Property* _property, EntityBase* root);

	virtual void addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator, EntityBase* root);
	virtual void readFromJsonObject(const ot::ConstJsonObject& object, EntityBase* root);

	void setToolTip(const std::string& _toolTip) { m_toolTip = _toolTip; };
	const std::string& getToolTip() const { return m_toolTip; };

	void setReadOnly(bool _flag) { m_readOnly = _flag; };
	bool getReadOnly() const { return m_readOnly; };

	void setProtected(bool _flag) { m_protectedProperty = _flag; };
	bool getProtected() const { return m_protectedProperty; };

	void setVisible(bool _flag) { m_visible = _flag; };
	bool getVisible() const { return m_visible; };
	
	void setErrorState(bool _flag) { m_errorState = _flag; };
	bool getErrorState() const { return m_errorState; };

	virtual void copySettings(EntityPropertiesBase *other, EntityBase *root);

	EntityPropertiesBase& operator=(const EntityPropertiesBase &other);

protected:
	void setupPropertyData(ot::PropertyGridCfg& _configuration, ot::Property* _property) const;

private:
	void setMultipleValues() { m_multipleValues = true; }

	EntityProperties* m_container;
	bool m_needsUpdateFlag;
	std::string m_name;
	std::string m_group;
	std::string m_toolTip;
	bool m_multipleValues;
	bool m_readOnly;
	bool m_protectedProperty;
	bool m_visible;
	bool m_errorState;
};

// ################################################################################################################################################################

class OT_MODELENTITIES_API_EXPORT EntityPropertiesDouble : public EntityPropertiesBase
{
public:
	static std::string typeString() { return "double"; };

	EntityPropertiesDouble();
	virtual ~EntityPropertiesDouble() {};

	EntityPropertiesDouble(const std::string& _name, double _value);

	EntityPropertiesDouble(const EntityPropertiesDouble& _other);

	virtual EntityPropertiesBase *createCopy() const override { return new EntityPropertiesDouble(*this); };

	EntityPropertiesDouble& operator=(const EntityPropertiesDouble& _other);

	virtual eType getType() const override { return DOUBLE; };
	virtual std::string getTypeString() const override { return EntityPropertiesDouble::typeString(); };

	void setValue(double _value);
	double getValue() const { return m_value; };

	void setRange(double _min, double _max);
	void setMin(double _min);
	double getMin() const { return m_min; };
	void setMax(double _max);
	double getMax() const { return m_max; };

	void setAllowCustomValues(bool _allowCustomValues);
	bool getAllowCustomValues() const { return m_allowCustomValues; };

	void setSuffix(const std::string& _suffix) { m_suffix = _suffix; };
	const std::string& getSuffix() const { return m_suffix; };

	virtual bool hasSameValue(EntityPropertiesBase *other) const override;

	virtual void addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase *root) override;
	virtual void setFromConfiguration(const ot::Property* _property, EntityBase* root) override;

	virtual void addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator, EntityBase* root) override;
	virtual void readFromJsonObject(const ot::ConstJsonObject& object, EntityBase* root) override;

	virtual void copySettings(EntityPropertiesBase *other, EntityBase *root) override;

	static EntityPropertiesDouble* createProperty(const std::string& _group, const std::string& _name, double _defaultValue, const std::string& _defaultCategory, EntityProperties& _properties);
	static EntityPropertiesDouble* createProperty(const std::string& _group, const std::string& _name, double _defaultValue, double _minValue, double _maxValue, const std::string& _defaultCategory, EntityProperties& _properties);

private:
	double m_value;
	bool m_allowCustomValues;
	double m_min;
	double m_max;
	std::string m_suffix;
};

// ################################################################################################################################################################

class OT_MODELENTITIES_API_EXPORT EntityPropertiesInteger : public EntityPropertiesBase
{
public:
	static std::string typeString() { return "integer"; };

	EntityPropertiesInteger();
	virtual ~EntityPropertiesInteger() {};

	EntityPropertiesInteger(const std::string& _name, long _value);

	EntityPropertiesInteger(const EntityPropertiesInteger& _other);

	virtual EntityPropertiesBase *createCopy() const override { return new EntityPropertiesInteger(*this); };

	EntityPropertiesInteger& operator=(const EntityPropertiesInteger& _other);

	virtual eType getType() const override { return INTEGER; };
	virtual std::string getTypeString() const override { return EntityPropertiesInteger::typeString(); };

	void setValue(long _value);
	long getValue() const { return m_value; };

	void setRange(long _min, long _max);
	void setMin(long _min);
	long getMin() const { return m_min; };
	void setMax(long _max);
	long getMax() const { return m_max; };

	void setAllowCustomValues(bool _allowCustomValues);
	bool getAllowCustomValues() const { return m_allowCustomValues; };

	void setSuffix(const std::string& _suffix) { m_suffix = _suffix; };
	const std::string& getSuffix() const { return m_suffix; };

	virtual bool hasSameValue(EntityPropertiesBase *other) const override;

	virtual void addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase *root) override;
	virtual void setFromConfiguration(const ot::Property* _property, EntityBase* root) override;

	virtual void addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator, EntityBase* _root) override;
	virtual void readFromJsonObject(const ot::ConstJsonObject& object, EntityBase* root) override;

	virtual void copySettings(EntityPropertiesBase *other, EntityBase *root) override;

	static EntityPropertiesInteger* createProperty(const std::string& _group, const std::string& _name, long _defaultValue, const std::string& _defaultCategory, EntityProperties& _properties);
	static EntityPropertiesInteger* createProperty(const std::string& _group, const std::string& _name, long _defaultValue, long _minValue, long _maxValue, const std::string& _defaultCategory, EntityProperties& _properties);

private:
	long m_value;
	bool m_allowCustomValues;
	long m_min;
	long m_max;
	std::string m_suffix;
};

// ################################################################################################################################################################

class OT_MODELENTITIES_API_EXPORT EntityPropertiesBoolean : public EntityPropertiesBase
{
public:
	static std::string typeString() { return "boolean"; };

	EntityPropertiesBoolean() : m_value(false) {};
	virtual ~EntityPropertiesBoolean() {};

	EntityPropertiesBoolean(const std::string &n, bool v) : m_value(v) { setName(n); };

	EntityPropertiesBoolean(const EntityPropertiesBoolean &other) : EntityPropertiesBase(other) { m_value = other.m_value; };

	virtual EntityPropertiesBase *createCopy() const override { return new EntityPropertiesBoolean(*this); };

	EntityPropertiesBoolean& operator=(const EntityPropertiesBoolean &other) { if (&other != this) { EntityPropertiesBase::operator=(other); m_value = other.getValue(); }; return *this; };

	virtual eType getType() const override { return BOOLEAN; };
	virtual std::string getTypeString() const override { return EntityPropertiesBoolean::typeString(); };

	void setValue(bool b) { if (m_value != b) setNeedsUpdate(); m_value = b; };
	bool getValue() const { return m_value; };

	virtual bool hasSameValue(EntityPropertiesBase *other) const override;

	virtual void addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase *root) override;
	virtual void setFromConfiguration(const ot::Property* _property, EntityBase* root) override;

	virtual void addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator, EntityBase* _root) override;
	virtual void readFromJsonObject(const ot::ConstJsonObject& object, EntityBase* root) override;

	virtual void copySettings(EntityPropertiesBase *other, EntityBase *root);

	static EntityPropertiesBoolean* createProperty(const std::string &group, const std::string &name, bool defaultValue, const std::string &defaultCategory, EntityProperties &properties);

private:
	bool m_value;
};

// ################################################################################################################################################################

class OT_MODELENTITIES_API_EXPORT EntityPropertiesString : public EntityPropertiesBase
{
public:
	static std::string typeString() { return "string"; };

	EntityPropertiesString() {};
	virtual ~EntityPropertiesString() {};

	EntityPropertiesString(const std::string &n, const std::string &v) : m_value(v) { setName(n); };

	EntityPropertiesString(const EntityPropertiesString &other) : EntityPropertiesBase(other) { m_value = other.m_value; };

	virtual EntityPropertiesBase *createCopy() const override { return new EntityPropertiesString(*this); };

	EntityPropertiesString& operator=(const EntityPropertiesString &other) { if (&other != this) { EntityPropertiesBase::operator=(other); m_value =other.getValue(); }; return *this; };

	virtual eType getType() const override { return STRING; };
	virtual std::string getTypeString() const override { return EntityPropertiesString::typeString(); };

	void setValue(const std::string &s) { if (m_value != s) setNeedsUpdate(); m_value = s; };
	const std::string& getValue() const { return m_value; };

	virtual bool hasSameValue(EntityPropertiesBase *other) const override;

	virtual void addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase *root) override;
	virtual void setFromConfiguration(const ot::Property* _property, EntityBase* root) override;

	virtual void addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator, EntityBase* _root) override;
	virtual void readFromJsonObject(const ot::ConstJsonObject& object, EntityBase* root) override;

	virtual void copySettings(EntityPropertiesBase *other, EntityBase *root);

	static EntityPropertiesString* createProperty(const std::string &group, const std::string &name, const std::string &defaultValue, const std::string &defaultCategory, EntityProperties &properties);

private:
	std::string m_value;
};

// ################################################################################################################################################################

class OT_MODELENTITIES_API_EXPORT EntityPropertiesSelection : public EntityPropertiesBase
{
public:
	static std::string typeString() { return "selection"; };

	EntityPropertiesSelection() {};
	virtual ~EntityPropertiesSelection() {};

	EntityPropertiesSelection(const EntityPropertiesSelection &other) : EntityPropertiesBase(other) { m_options = other.m_options; m_value = other.m_value; };

	virtual EntityPropertiesBase *createCopy() const override { return new EntityPropertiesSelection(*this); };

	EntityPropertiesSelection& operator=(const EntityPropertiesSelection &other) { if (&other != this) { assert(checkCompatibilityOfSettings(other)); EntityPropertiesBase::operator=(other); m_value = other.getValue(); }; return *this; };

	virtual eType getType() const override { return SELECTION; };
	virtual std::string getTypeString() const override { return EntityPropertiesSelection::typeString(); };

	virtual bool isCompatible(EntityPropertiesBase *other) const override;

	bool setValue(const std::string &s);
	const std::string& getValue()const { return m_value; };

	void clearOptions() { m_options.clear(); };
	void addOption(const std::string &option) { assert(std::find(m_options.begin(), m_options.end(), option) == m_options.end());  m_options.push_back(option); }
	void resetOptions(const std::list<std::string>& _options);
	const std::vector<std::string> &getOptions() { return m_options; };

	virtual bool hasSameValue(EntityPropertiesBase *other) const override;

	virtual void addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase *root) override;
	virtual void setFromConfiguration(const ot::Property* _property, EntityBase* root) override;

	virtual void addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator, EntityBase* _root) override;
	virtual void readFromJsonObject(const ot::ConstJsonObject& object, EntityBase* root) override;

	virtual void copySettings(EntityPropertiesBase *other, EntityBase *root);

	static EntityPropertiesSelection* createProperty(const std::string &group, const std::string &name, std::list<std::string>& options, const std::string &defaultValue, const std::string &defaultCategory, EntityProperties &properties);
	static EntityPropertiesSelection* createProperty(const std::string &group, const std::string &name, std::list<std::string>&& options, const std::string &defaultValue, const std::string &defaultCategory, EntityProperties &properties);

private:
	bool checkCompatibilityOfSettings(const EntityPropertiesSelection &other) const;

	std::vector<std::string> m_options;
	std::string m_value;
};

// ################################################################################################################################################################

class OT_MODELENTITIES_API_EXPORT EntityPropertiesColor : public EntityPropertiesBase
{
public:
	static std::string typeString() { return "color"; };

	EntityPropertiesColor() : m_color{ 0.0, 0.0, 0.0 } {};

	EntityPropertiesColor(const EntityPropertiesColor &other) : EntityPropertiesBase(other) { m_color[0] = other.getColorR(); m_color[1] = other.getColorG(); m_color[2] = other.getColorB(); };

	EntityPropertiesColor(const std::string &n, double r, double g, double b) : m_color{ r, g, b } { setName(n); };
	virtual ~EntityPropertiesColor() {};

	virtual EntityPropertiesBase *createCopy() const override { return new EntityPropertiesColor(*this); };

	EntityPropertiesColor& operator=(const EntityPropertiesColor &other);

	virtual eType getType() const override { return COLOR; };
	virtual std::string getTypeString() const override { return EntityPropertiesColor::typeString(); };

	void setColorR(double c) { if (m_color[0] != c) setNeedsUpdate(); m_color[0] = c; };
	void setColorG(double c) { if (m_color[1] != c) setNeedsUpdate(); m_color[1] = c; };
	void setColorB(double c) { if (m_color[2] != c) setNeedsUpdate(); m_color[2] = c; };

	void setColorRGB(double r, double g, double b) { if (m_color[0] != r || m_color[1] != g || m_color[2] != b) setNeedsUpdate(); m_color[0] = r; m_color[1] = g; m_color[2] = b; };

	double getColorR() const { return m_color[0]; };
	double getColorG() const { return m_color[1]; };
	double getColorB() const { return m_color[2]; };

	virtual bool hasSameValue(EntityPropertiesBase *other) const override;

	virtual void addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase *root) override;
	virtual void setFromConfiguration(const ot::Property* _property, EntityBase* root) override;

	virtual void addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator, EntityBase* _root) override;
	virtual void readFromJsonObject(const ot::ConstJsonObject& object, EntityBase* root) override;

	virtual void copySettings(EntityPropertiesBase *other, EntityBase *root);

	static EntityPropertiesColor* createProperty(const std::string &group, const std::string &name, std::vector<int> defaultValue, const std::string &defaultCategory, EntityProperties &properties);

private:
	double m_color[3];
};

// ################################################################################################################################################################

class OT_MODELENTITIES_API_EXPORT EntityPropertiesEntityList : public EntityPropertiesBase
{
public:
	static std::string typeString() { return "entitylist"; };

	EntityPropertiesEntityList() : m_entityContainerID(0), m_valueID(0) {};

	EntityPropertiesEntityList(const EntityPropertiesEntityList &other) : EntityPropertiesBase(other) { m_entityContainerName = other.getEntityContainerName(); m_entityContainerID = other.getEntityContainerID(); m_valueName = other.getValueName(); m_valueID = other.getValueID(); };
	EntityPropertiesEntityList(const std::string &n, const std::string &contName, ot::UID contID, const std::string &valName, ot::UID valID) : m_entityContainerName(contName), m_entityContainerID(contID), m_valueName(valName), m_valueID(valID) { setName(n); };

	virtual ~EntityPropertiesEntityList() {};

	virtual EntityPropertiesBase *createCopy() const override { return new EntityPropertiesEntityList(*this); };

	EntityPropertiesEntityList& operator=(const EntityPropertiesEntityList &other);

	virtual eType getType() const override { return ENTITYLIST; };
	virtual std::string getTypeString() const override { return EntityPropertiesEntityList::typeString(); };

	virtual bool hasSameValue(EntityPropertiesBase *other) const override;

	virtual void addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase *root) override;
	virtual void setFromConfiguration(const ot::Property* _property, EntityBase* root) override;

	virtual void addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator, EntityBase* _root) override;
	virtual void readFromJsonObject(const ot::ConstJsonObject& object, EntityBase* root) override;

	virtual void copySettings(EntityPropertiesBase *other, EntityBase *root);

	void setEntityContainerName(const std::string &containerName) { if (m_entityContainerName != containerName) setNeedsUpdate(); m_entityContainerName = containerName; };
	void setEntityContainerID(ot::UID containerID) { if (m_entityContainerID != containerID) setNeedsUpdate(); m_entityContainerID = containerID; };

	void setValueName(const std::string &vname) { if (m_valueName != vname) setNeedsUpdate(); m_valueName = vname; };
	void setValueID(ot::UID vid) { if (m_valueID != vid) setNeedsUpdate(); m_valueID = vid; };

	std::string getEntityContainerName() const { return m_entityContainerName; };
	ot::UID getEntityContainerID() const { return m_entityContainerID; };

	std::string getValueName() const { return m_valueName; };
	ot::UID getValueID() const { return m_valueID; };

	static EntityPropertiesEntityList* createProperty(const std::string &group, const std::string &name, const std::string &contName, ot::UID contID, const std::string &valName, ot::UID valID, const std::string &defaultCategory, EntityProperties &properties);

private:

	//! \brief Will update the ValueName, ValueID, ContainerName and ContainerID.
	//! This method will check if the value and container IDs are correct and set the names respectivly.
	//! If the IDs are incorrect the IDs will be determined by the name.
	void updateValueAndContainer(EntityBase* _root, std::list<std::string>& _containerOptions);

	EntityContainer *findContainerFromID(EntityBase *root, ot::UID entityID);
	EntityContainer *findContainerFromName(EntityBase *root, const std::string &entityName);
	EntityBase *findEntityFromName(EntityBase *root, const std::string &entityName);
	EntityBase *findEntityFromID(EntityBase *root, ot::UID entityID);

	std::string m_entityContainerName;
	ot::UID m_entityContainerID;

	std::string m_valueName;
	ot::UID m_valueID;
};

// ################################################################################################################################################################

class OT_MODELENTITIES_API_EXPORT EntityPropertiesProjectList : public EntityPropertiesBase
{
public:
	static std::string typeString() { return "projectlist"; };

	EntityPropertiesProjectList() {};
	EntityPropertiesProjectList(const std::string& name) { setName(name); }
	virtual EntityPropertiesBase* createCopy() const override { return new EntityPropertiesProjectList(*this); };
	virtual eType getType() const override { return PROJECTLIST; };
	virtual std::string getTypeString() const override { return EntityPropertiesProjectList::typeString(); };

	virtual void copySettings(EntityPropertiesBase* other, EntityBase* root);

	virtual bool hasSameValue(EntityPropertiesBase* other) const override { return true; };
	
	virtual void addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase* root) override;
	virtual void setFromConfiguration(const ot::Property* _property, EntityBase* root) override;

	virtual void addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator, EntityBase* _root) override;
	virtual void readFromJsonObject(const ot::ConstJsonObject& object, EntityBase* root) override;

	void setValue(const std::string& value) { m_value = value; }
	std::string getValue() const {return m_value;}
private:
	std::string m_value;
};

// ################################################################################################################################################################

class OT_MODELENTITIES_API_EXPORT EntityPropertiesGuiPainter : public EntityPropertiesBase
{
public:
	static std::string typeString() { return "guipainter"; };

	EntityPropertiesGuiPainter();
	virtual ~EntityPropertiesGuiPainter();

	//! \brief Constructor.
	//! The entity takes ownership of the painter.
	EntityPropertiesGuiPainter(const std::string& n, ot::Painter2D* _painter) : m_painter(_painter) { setName(n); };

	EntityPropertiesGuiPainter(const EntityPropertiesGuiPainter& other);

	virtual EntityPropertiesBase* createCopy() const override { return new EntityPropertiesGuiPainter(*this); };

	EntityPropertiesGuiPainter& operator=(const EntityPropertiesGuiPainter& other);

	virtual eType getType() const override { return GUIPAINTER; };
	virtual std::string getTypeString() const override { return EntityPropertiesGuiPainter::typeString(); };

	void setValue(const ot::Painter2D* _painter);

	//! \brief Returns the current painter.
	//! The entity keeps ownership of the painter
	const ot::Painter2D* getValue() const { return m_painter; };

	void setFilter(const ot::Painter2DDialogFilter& _filter) { m_filter = _filter; };
	const ot::Painter2DDialogFilter& getFilter() const { return m_filter; };

	virtual bool hasSameValue(EntityPropertiesBase* other) const override;

	virtual void addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase* root) override;
	virtual void setFromConfiguration(const ot::Property* _property, EntityBase* root) override;

	virtual void addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator, EntityBase* _root) override;
	virtual void readFromJsonObject(const ot::ConstJsonObject& object, EntityBase* root) override;

	virtual void copySettings(EntityPropertiesBase* other, EntityBase* root);

	static EntityPropertiesGuiPainter* createProperty(const std::string& group, const std::string& name, ot::Painter2D* _defaultPainter, const std::string& defaultCategory, EntityProperties& properties);

private:
	ot::Painter2D* m_painter;
	ot::Painter2DDialogFilter m_filter;
};