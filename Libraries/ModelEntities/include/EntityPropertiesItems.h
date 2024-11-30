#pragma once
#pragma warning(disable : 4251)

// OpenTwin header
#include "OTGui/PropertyGridCfg.h"
#include "OldTreeIcon.h"

// std header
#include <string>
#include <vector>

class EntityBase;
class EntityContainer;
class EntityProperties;

namespace ot { class Property; };
namespace ot { class Painter2D; };

class __declspec(dllexport) EntityPropertiesBase
{
public:
	EntityPropertiesBase();
	virtual ~EntityPropertiesBase() {};

	EntityPropertiesBase(const EntityPropertiesBase &other);

	virtual EntityPropertiesBase *createCopy(void) = 0;

	void setContainer(EntityProperties *c) { m_container = c; };

	void setName(const std::string& _name) { m_name = _name; };
	const std::string& getName(void) const { return m_name; };

	void setGroup(const std::string& _group) { m_group = _group; };
	const std::string& getGroup(void) const { return m_group; };

	enum eType { DOUBLE, INTEGER, BOOLEAN, STRING, SELECTION, COLOR, ENTITYLIST, PROJECTLIST, GUIPAINTER };
	virtual eType getType(void) = 0;

	void resetNeedsUpdate(void) { m_needsUpdateFlag = false; };
	bool needsUpdate(void);
	void setNeedsUpdate(void);

	bool hasMultipleValues(void) { return m_multipleValues; };
	void setHasMultipleValues(bool b) { m_multipleValues = b; };

	virtual bool isCompatible(EntityPropertiesBase *other) { return true; };
	virtual bool hasSameValue(EntityPropertiesBase *other) = 0;

	virtual void addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase *root) = 0;
	virtual void setFromConfiguration(const ot::Property* _property, EntityBase* root) = 0;

	virtual void addToJsonDocument(ot::JsonDocument& jsonDoc, EntityBase* root) = 0;
	virtual void readFromJsonObject(const ot::ConstJsonObject& object, EntityBase* root) = 0;

	void setReadOnly(bool flag) { m_readOnly = flag; };
	bool getReadOnly(void) { return m_readOnly; };

	void setProtected(bool flag) { m_protectedProperty = flag; };
	bool getProtected(void) { return m_protectedProperty; };

	void setVisible(bool flag) { m_visible = flag; };
	bool getVisible(void) { return m_visible; };
	
	void setErrorState(bool flag) { m_errorState = flag; };
	bool getErrorState(void) { return m_errorState; };

	virtual void copySettings(EntityPropertiesBase *other, EntityBase *root);

	EntityPropertiesBase& operator=(const EntityPropertiesBase &other);

protected:
	void setupPropertyData(ot::PropertyGridCfg& _configuration, ot::Property* _property);
	void addBaseDataToJsonDocument(ot::JsonValue& container, ot::JsonAllocator& allocator, const std::string& type);

private:
	void setMultipleValues(void) { m_multipleValues = true; }

	EntityProperties* m_container;
	bool m_needsUpdateFlag;
	std::string m_name;
	std::string m_group;
	bool m_multipleValues;
	bool m_readOnly;
	bool m_protectedProperty;
	bool m_visible;
	bool m_errorState;
};

// ################################################################################################################################################################

class __declspec(dllexport) EntityPropertiesDouble : public EntityPropertiesBase
{
public:
	EntityPropertiesDouble();
	virtual ~EntityPropertiesDouble() {};

	EntityPropertiesDouble(const std::string& _name, double _value);

	EntityPropertiesDouble(const EntityPropertiesDouble& _other);

	virtual EntityPropertiesBase *createCopy(void) override { return new EntityPropertiesDouble(*this); };

	EntityPropertiesDouble& operator=(const EntityPropertiesDouble& _other);

	virtual eType getType(void) override { return DOUBLE; };

	void setValue(double _value);
	double getValue(void) const { return m_value; };

	void setRange(double _min, double _max);
	void setMin(double _min);
	double getMin(void) const { return m_min; };
	void setMax(double _max);
	double getMax(void) const { return m_max; };

	void setAllowCustomValues(bool _allowCustomValues);
	bool getAllowCustomValues(void) const { return m_allowCustomValues; };

	virtual bool hasSameValue(EntityPropertiesBase *other) override;

	virtual void addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase *root) override;
	virtual void setFromConfiguration(const ot::Property* _property, EntityBase* root) override;

	virtual void addToJsonDocument(ot::JsonDocument& jsonDoc, EntityBase* root) override;
	virtual void readFromJsonObject(const ot::ConstJsonObject& object, EntityBase* root) override;

	virtual void copySettings(EntityPropertiesBase *other, EntityBase *root);

	static EntityPropertiesDouble* createProperty(const std::string& _group, const std::string& _name, double _defaultValue, const std::string& _defaultCategory, EntityProperties& _properties);
	static EntityPropertiesDouble* createProperty(const std::string& _group, const std::string& _name, double _defaultValue, double _minValue, double _maxValue, const std::string& _defaultCategory, EntityProperties& _properties);

private:
	double m_value;
	bool m_allowCustomValues;
	double m_min;
	double m_max;
};

// ################################################################################################################################################################

class __declspec(dllexport) EntityPropertiesInteger : public EntityPropertiesBase
{
public:
	EntityPropertiesInteger();
	virtual ~EntityPropertiesInteger() {};

	EntityPropertiesInteger(const std::string& _name, long _value);

	EntityPropertiesInteger(const EntityPropertiesInteger& _other);

	virtual EntityPropertiesBase *createCopy(void) override { return new EntityPropertiesInteger(*this); };

	EntityPropertiesInteger& operator=(const EntityPropertiesInteger& _other);

	virtual eType getType(void) override { return INTEGER; };

	void setValue(long _value);
	long getValue(void) const { return m_value; };

	void setRange(long _min, long _max);
	void setMin(long _min);
	long getMin(void) const { return m_min; };
	void setMax(long _max);
	long getMax(void) const { return m_max; };

	void setAllowCustomValues(bool _allowCustomValues);
	bool getAllowCustomValues(void) const { return m_allowCustomValues; };

	virtual bool hasSameValue(EntityPropertiesBase *other) override;

	virtual void addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase *root) override;
	virtual void setFromConfiguration(const ot::Property* _property, EntityBase* root) override;

	virtual void addToJsonDocument(ot::JsonDocument& jsonDoc, EntityBase* root) override;
	virtual void readFromJsonObject(const ot::ConstJsonObject& object, EntityBase* root) override;

	virtual void copySettings(EntityPropertiesBase *other, EntityBase *root);

	static EntityPropertiesInteger* createProperty(const std::string& _group, const std::string& _name, long _defaultValue, const std::string& _defaultCategory, EntityProperties& _properties);
	static EntityPropertiesInteger* createProperty(const std::string& _group, const std::string& _name, long _defaultValue, long _minValue, long _maxValue, const std::string& _defaultCategory, EntityProperties& _properties);

private:
	long m_value;
	bool m_allowCustomValues;
	long m_min;
	long m_max;
};

// ################################################################################################################################################################

class __declspec(dllexport) EntityPropertiesBoolean : public EntityPropertiesBase
{
public:
	EntityPropertiesBoolean() : m_value(false) {};
	virtual ~EntityPropertiesBoolean() {};

	EntityPropertiesBoolean(const std::string &n, bool v) : m_value(v) { setName(n); };

	EntityPropertiesBoolean(const EntityPropertiesBoolean &other) : EntityPropertiesBase(other) { m_value = other.m_value; };

	virtual EntityPropertiesBase *createCopy(void) override { return new EntityPropertiesBoolean(*this); };

	EntityPropertiesBoolean& operator=(const EntityPropertiesBoolean &other) { if (&other != this) { EntityPropertiesBase::operator=(other); m_value = other.getValue(); }; return *this; };

	virtual eType getType(void) override { return BOOLEAN; };

	void setValue(bool b) { if (m_value != b) setNeedsUpdate(); m_value = b; };
	bool getValue(void) const { return m_value; };

	virtual bool hasSameValue(EntityPropertiesBase *other) override;

	virtual void addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase *root) override;
	virtual void setFromConfiguration(const ot::Property* _property, EntityBase* root) override;

	virtual void addToJsonDocument(ot::JsonDocument& jsonDoc, EntityBase* root) override;
	virtual void readFromJsonObject(const ot::ConstJsonObject& object, EntityBase* root) override;

	virtual void copySettings(EntityPropertiesBase *other, EntityBase *root);

	static EntityPropertiesBoolean* createProperty(const std::string &group, const std::string &name, bool defaultValue, const std::string &defaultCategory, EntityProperties &properties);

private:
	bool m_value;
};

// ################################################################################################################################################################

class __declspec(dllexport) EntityPropertiesString : public EntityPropertiesBase
{
public:
	EntityPropertiesString() {};
	virtual ~EntityPropertiesString() {};

	EntityPropertiesString(const std::string &n, const std::string &v) : m_value(v) { setName(n); };

	EntityPropertiesString(const EntityPropertiesString &other) : EntityPropertiesBase(other) { m_value = other.m_value; };

	virtual EntityPropertiesBase *createCopy(void) override { return new EntityPropertiesString(*this); };

	EntityPropertiesString& operator=(const EntityPropertiesString &other) { if (&other != this) { EntityPropertiesBase::operator=(other); m_value =other.getValue(); }; return *this; };

	virtual eType getType(void) override { return STRING; };

	void setValue(const std::string &s) { if (m_value != s) setNeedsUpdate(); m_value = s; };
	const std::string& getValue(void) const { return m_value; };

	virtual bool hasSameValue(EntityPropertiesBase *other) override;

	virtual void addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase *root) override;
	virtual void setFromConfiguration(const ot::Property* _property, EntityBase* root) override;

	virtual void addToJsonDocument(ot::JsonDocument& jsonDoc, EntityBase* root) override;
	virtual void readFromJsonObject(const ot::ConstJsonObject& object, EntityBase* root) override;

	virtual void copySettings(EntityPropertiesBase *other, EntityBase *root);

	static EntityPropertiesString* createProperty(const std::string &group, const std::string &name, const std::string &defaultValue, const std::string &defaultCategory, EntityProperties &properties);

private:
	std::string m_value;
};

// ################################################################################################################################################################

class __declspec(dllexport) EntityPropertiesSelection : public EntityPropertiesBase
{
public:
	EntityPropertiesSelection() {};
	virtual ~EntityPropertiesSelection() {};

	EntityPropertiesSelection(const EntityPropertiesSelection &other) : EntityPropertiesBase(other) { m_options = other.m_options; m_value = other.m_value; };

	virtual EntityPropertiesBase *createCopy(void) override { return new EntityPropertiesSelection(*this); };

	EntityPropertiesSelection& operator=(const EntityPropertiesSelection &other) { if (&other != this) { assert(checkCompatibilityOfSettings(other)); EntityPropertiesBase::operator=(other); m_value = other.getValue(); }; return *this; };

	virtual eType getType(void) override { return SELECTION; };

	virtual bool isCompatible(EntityPropertiesBase *other) override;

	bool setValue(const std::string &s);
	const std::string& getValue(void)const { return m_value; };

	void clearOptions() { m_options.clear(); };
	void addOption(const std::string &option) { assert(std::find(m_options.begin(), m_options.end(), option) == m_options.end());  m_options.push_back(option); }
	void resetOptions(std::list<std::string>& _options);
	const std::vector<std::string> &getOptions(void) { return m_options; };

	virtual bool hasSameValue(EntityPropertiesBase *other) override;

	virtual void addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase *root) override;
	virtual void setFromConfiguration(const ot::Property* _property, EntityBase* root) override;

	virtual void addToJsonDocument(ot::JsonDocument& jsonDoc, EntityBase* root) override;
	virtual void readFromJsonObject(const ot::ConstJsonObject& object, EntityBase* root) override;

	virtual void copySettings(EntityPropertiesBase *other, EntityBase *root);

	static EntityPropertiesSelection* createProperty(const std::string &group, const std::string &name, std::list<std::string>& options, const std::string &defaultValue, const std::string &defaultCategory, EntityProperties &properties);
	static EntityPropertiesSelection* createProperty(const std::string &group, const std::string &name, std::list<std::string>&& options, const std::string &defaultValue, const std::string &defaultCategory, EntityProperties &properties);

private:
	bool checkCompatibilityOfSettings(const EntityPropertiesSelection &other);

	std::vector<std::string> m_options;
	std::string m_value;
};

// ################################################################################################################################################################

class __declspec(dllexport) EntityPropertiesColor : public EntityPropertiesBase
{
public:
	EntityPropertiesColor() : m_color{ 0.0, 0.0, 0.0 } {};

	EntityPropertiesColor(const EntityPropertiesColor &other) : EntityPropertiesBase(other) { m_color[0] = other.getColorR(); m_color[1] = other.getColorG(); m_color[2] = other.getColorB(); };

	EntityPropertiesColor(const std::string &n, double r, double g, double b) : m_color{ r, g, b } { setName(n); };
	virtual ~EntityPropertiesColor() {};

	virtual EntityPropertiesBase *createCopy(void) override { return new EntityPropertiesColor(*this); };

	EntityPropertiesColor& operator=(const EntityPropertiesColor &other);

	virtual eType getType(void) override { return COLOR; };

	void setColorR(double c) { if (m_color[0] != c) setNeedsUpdate(); m_color[0] = c; };
	void setColorG(double c) { if (m_color[1] != c) setNeedsUpdate(); m_color[1] = c; };
	void setColorB(double c) { if (m_color[2] != c) setNeedsUpdate(); m_color[2] = c; };

	void setColorRGB(double r, double g, double b) { if (m_color[0] != r || m_color[1] != g || m_color[2] != b) setNeedsUpdate(); m_color[0] = r; m_color[1] = g; m_color[2] = b; };

	double getColorR(void) const { return m_color[0]; };
	double getColorG(void) const { return m_color[1]; };
	double getColorB(void) const { return m_color[2]; };

	virtual bool hasSameValue(EntityPropertiesBase *other) override;

	virtual void addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase *root) override;
	virtual void setFromConfiguration(const ot::Property* _property, EntityBase* root) override;

	virtual void addToJsonDocument(ot::JsonDocument& jsonDoc, EntityBase* root) override;
	virtual void readFromJsonObject(const ot::ConstJsonObject& object, EntityBase* root) override;

	virtual void copySettings(EntityPropertiesBase *other, EntityBase *root);

	static EntityPropertiesColor* createProperty(const std::string &group, const std::string &name, std::vector<int> defaultValue, const std::string &defaultCategory, EntityProperties &properties);

private:
	double m_color[3];
};

// ################################################################################################################################################################

class __declspec(dllexport) EntityPropertiesEntityList : public EntityPropertiesBase
{
public:
	EntityPropertiesEntityList() : m_entityContainerID(0), m_valueID(0) {};

	EntityPropertiesEntityList(const EntityPropertiesEntityList &other) : EntityPropertiesBase(other) { m_entityContainerName = other.getEntityContainerName(); m_entityContainerID = other.getEntityContainerID(); m_valueName = other.getValueName(); m_valueID = other.getValueID(); };
	EntityPropertiesEntityList(const std::string &n, const std::string &contName, ot::UID contID, const std::string &valName, ot::UID valID) : m_entityContainerName(contName), m_entityContainerID(contID), m_valueName(valName), m_valueID(valID) { setName(n); };

	virtual ~EntityPropertiesEntityList() {};

	virtual EntityPropertiesBase *createCopy(void) override { return new EntityPropertiesEntityList(*this); };

	EntityPropertiesEntityList& operator=(const EntityPropertiesEntityList &other);

	virtual eType getType(void) override { return ENTITYLIST; };

	virtual bool hasSameValue(EntityPropertiesBase *other) override;

	virtual void addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase *root) override;
	virtual void setFromConfiguration(const ot::Property* _property, EntityBase* root) override;

	virtual void addToJsonDocument(ot::JsonDocument& jsonDoc, EntityBase* root) override;
	virtual void readFromJsonObject(const ot::ConstJsonObject& object, EntityBase* root) override;

	virtual void copySettings(EntityPropertiesBase *other, EntityBase *root);

	void setEntityContainerName(const std::string &containerName) { if (m_entityContainerName != containerName) setNeedsUpdate(); m_entityContainerName = containerName; };
	void setEntityContainerID(ot::UID containerID) { if (m_entityContainerID != containerID) setNeedsUpdate(); m_entityContainerID = containerID; };

	void setValueName(const std::string &vname) { if (m_valueName != vname) setNeedsUpdate(); m_valueName = vname; };
	void setValueID(ot::UID vid) { if (m_valueID != vid) setNeedsUpdate(); m_valueID = vid; };

	std::string getEntityContainerName(void) const { return m_entityContainerName; };
	ot::UID getEntityContainerID(void) const { return m_entityContainerID; };

	std::string getValueName(void) const { return m_valueName; };
	ot::UID getValueID(void) const { return m_valueID; };

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

class __declspec(dllexport) EntityPropertiesProjectList : public EntityPropertiesBase
{
public:
	EntityPropertiesProjectList() {};
	EntityPropertiesProjectList(const std::string& name) { setName(name); }
	virtual EntityPropertiesBase* createCopy(void) override { return new EntityPropertiesProjectList(*this); };
	virtual eType getType(void) override { return PROJECTLIST; };
	virtual void copySettings(EntityPropertiesBase* other, EntityBase* root);

	virtual bool hasSameValue(EntityPropertiesBase* other) override { return true; };
	
	virtual void addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase* root) override;
	virtual void setFromConfiguration(const ot::Property* _property, EntityBase* root) override;

	virtual void addToJsonDocument(ot::JsonDocument& jsonDoc, EntityBase* root) override;
	virtual void readFromJsonObject(const ot::ConstJsonObject& object, EntityBase* root) override;

	void setValue(std::string& value) { m_value = value; }
	std::string getValue() const {return m_value;}
private:
	std::string m_value;
};

// ################################################################################################################################################################

class __declspec(dllexport) EntityPropertiesGuiPainter : public EntityPropertiesBase
{
public:
	EntityPropertiesGuiPainter();
	virtual ~EntityPropertiesGuiPainter();

	//! \brief Constructor.
	//! The entity takes ownership of the painter.
	EntityPropertiesGuiPainter(const std::string& n, ot::Painter2D* _painter) : m_painter(_painter) { setName(n); };

	EntityPropertiesGuiPainter(const EntityPropertiesGuiPainter& other);

	virtual EntityPropertiesBase* createCopy(void) override { return new EntityPropertiesGuiPainter(*this); };

	EntityPropertiesGuiPainter& operator=(const EntityPropertiesGuiPainter& other);

	virtual eType getType(void) override { return GUIPAINTER; };

	void setValue(ot::Painter2D* _painter);

	//! \brief Returns the current painter.
	//! The entity keeps ownership of the painter
	const ot::Painter2D* getValue(void) const { return m_painter; };

	virtual bool hasSameValue(EntityPropertiesBase* other) override;

	virtual void addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase* root) override;
	virtual void setFromConfiguration(const ot::Property* _property, EntityBase* root) override;

	virtual void addToJsonDocument(ot::JsonDocument& jsonDoc, EntityBase* root) override;
	virtual void readFromJsonObject(const ot::ConstJsonObject& object, EntityBase* root) override;

	virtual void copySettings(EntityPropertiesBase* other, EntityBase* root);

	static EntityPropertiesGuiPainter* createProperty(const std::string& group, const std::string& name, ot::Painter2D* _defaultPainter, const std::string& defaultCategory, EntityProperties& properties);

private:
	ot::Painter2D* m_painter;
};