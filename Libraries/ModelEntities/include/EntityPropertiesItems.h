#pragma once
#pragma warning(disable : 4251)

#include <string>
#include <vector>

class EntityProperties;
class EntityBase;
class EntityContainer;

#include "rapidjson/document.h"

#include "Types.h"

class __declspec(dllexport) EntityPropertiesBase
{
public:
	EntityPropertiesBase() : container(nullptr), needsUpdateFlag(false), multipleValues(false), readOnly(false), protectedProperty(true), visible(true), errorState(false) {};
	virtual ~EntityPropertiesBase() {};

	EntityPropertiesBase(const EntityPropertiesBase &other);

	virtual EntityPropertiesBase *createCopy(void) = 0;

	void setContainer(EntityProperties *c) { container = c; };

	void setName(const std::string &n) { name = n; };
	const std::string &getName(void) { return name; };

	void setGroup(const std::string &g) { group = g; };
	const std::string &getGroup(void) { return group; };

	enum eType { DOUBLE, INTEGER, BOOLEAN, STRING, SELECTION, COLOR, ENTITYLIST, PROJECTLIST };
	virtual eType getType(void) = 0;

	void resetNeedsUpdate(void) { needsUpdateFlag = false; };
	bool needsUpdate(void);
	void setNeedsUpdate(void);

	bool hasMultipleValues(void) { return multipleValues; };
	void setHasMultipleValues(bool b) { multipleValues = b; };

	virtual bool isCompatible(EntityPropertiesBase *other) { return true; };
	virtual bool hasSameValue(EntityPropertiesBase *other) = 0;

	virtual void addToJsonDocument(rapidjson::Document &jsonDoc, EntityBase *root) = 0;
	virtual void readFromJsonObject(const rapidjson::Value &object) = 0;

	void setReadOnly(bool flag) { readOnly = flag; };
	bool getReadOnly(void) { return readOnly; };

	void setProtected(bool flag) { protectedProperty = flag; };
	bool getProtected(void) { return protectedProperty; };

	void setVisible(bool flag) { visible = flag; };
	bool getVisible(void) { return visible; };
	
	void setErrorState(bool flag) { errorState = flag; };
	bool getErrorState(void) { return errorState; };

	virtual void copySettings(EntityPropertiesBase *other, EntityBase *root);

	EntityPropertiesBase& operator=(const EntityPropertiesBase &other);

protected:
	void addToJsonDocument(rapidjson::Value &container, rapidjson::Document::AllocatorType &allocator, const std::string &type);

private:
	void setMultipleValues(void) { multipleValues = true; }

	EntityProperties *container;
	bool needsUpdateFlag;
	std::string name;
	std::string group;
	bool multipleValues;
	bool readOnly;
	bool protectedProperty;
	bool visible;
	bool errorState;
};

class __declspec(dllexport) EntityPropertiesDouble : public EntityPropertiesBase
{
public:
	EntityPropertiesDouble() : value(0.0) {};
	virtual ~EntityPropertiesDouble() {};

	EntityPropertiesDouble(const std::string &n, double v) : value(v) { setName(n); };

	EntityPropertiesDouble(const EntityPropertiesDouble &other) : EntityPropertiesBase(other) { value = other.value; };

	virtual EntityPropertiesBase *createCopy(void) override { return new EntityPropertiesDouble(*this); };

	EntityPropertiesDouble& operator=(const EntityPropertiesDouble &other) { if (&other != this) { EntityPropertiesBase::operator=(other); value = other.getValue(); }; return *this; };

	virtual eType getType(void) override { return DOUBLE; };

	void setValue(double v) { if (value != v) setNeedsUpdate();  value = v; };
	double getValue(void) const { return value; };

	virtual bool hasSameValue(EntityPropertiesBase *other) override;

	virtual void addToJsonDocument(rapidjson::Document &jsonDoc, EntityBase *root) override;
	virtual void readFromJsonObject(const rapidjson::Value &object) override;

	virtual void copySettings(EntityPropertiesBase *other, EntityBase *root);

	static void createProperty(const std::string &group, const std::string &name, double defaultValue, const std::string &defaultCategory, EntityProperties &properties);

private:
	double value;
};

class __declspec(dllexport) EntityPropertiesInteger : public EntityPropertiesBase
{
public:
	EntityPropertiesInteger() : value(0) {};
	virtual ~EntityPropertiesInteger() {};

	EntityPropertiesInteger(const std::string &n, long v) : value(v) { setName(n); };

	EntityPropertiesInteger(const EntityPropertiesInteger &other) : EntityPropertiesBase(other) { value = other.value; };

	virtual EntityPropertiesBase *createCopy(void) override { return new EntityPropertiesInteger(*this); };

	EntityPropertiesInteger& operator=(const EntityPropertiesInteger &other) { if (&other != this) { EntityPropertiesBase::operator=(other); value = other.getValue(); }; return *this; };

	virtual eType getType(void) override { return INTEGER; };

	void setValue(long v) { if (value != v) setNeedsUpdate(); value = v; };
	long getValue(void) const { return value; };

	virtual bool hasSameValue(EntityPropertiesBase *other) override;

	virtual void addToJsonDocument(rapidjson::Document &jsonDoc, EntityBase *root) override;
	virtual void readFromJsonObject(const rapidjson::Value &object) override;

	virtual void copySettings(EntityPropertiesBase *other, EntityBase *root);

	static void createProperty(const std::string &group, const std::string &name, long defaultValue, const std::string &defaultCategory, EntityProperties &properties);

private:
	long value;
};

class __declspec(dllexport) EntityPropertiesBoolean : public EntityPropertiesBase
{
public:
	EntityPropertiesBoolean() : value(false) {};
	virtual ~EntityPropertiesBoolean() {};

	EntityPropertiesBoolean(const std::string &n, bool v) : value(v) { setName(n); };

	EntityPropertiesBoolean(const EntityPropertiesBoolean &other) : EntityPropertiesBase(other) { value = other.value; };

	virtual EntityPropertiesBase *createCopy(void) override { return new EntityPropertiesBoolean(*this); };

	EntityPropertiesBoolean& operator=(const EntityPropertiesBoolean &other) { if (&other != this) { EntityPropertiesBase::operator=(other); value = other.getValue(); }; return *this; };

	virtual eType getType(void) override { return BOOLEAN; };

	void setValue(bool b) { if (value != b) setNeedsUpdate(); value = b; };
	bool getValue(void) const { return value; };

	virtual bool hasSameValue(EntityPropertiesBase *other) override;

	virtual void addToJsonDocument(rapidjson::Document &jsonDoc, EntityBase *root) override;
	virtual void readFromJsonObject(const rapidjson::Value &object) override;

	virtual void copySettings(EntityPropertiesBase *other, EntityBase *root);

	static void createProperty(const std::string &group, const std::string &name, bool defaultValue, const std::string &defaultCategory, EntityProperties &properties);

private:
	bool value;
};

class __declspec(dllexport) EntityPropertiesString : public EntityPropertiesBase
{
public:
	EntityPropertiesString() {};
	virtual ~EntityPropertiesString() {};

	EntityPropertiesString(const std::string &n, const std::string &v) : value(v) { setName(n); };

	EntityPropertiesString(const EntityPropertiesString &other) : EntityPropertiesBase(other) { value = other.value; };

	virtual EntityPropertiesBase *createCopy(void) override { return new EntityPropertiesString(*this); };

	EntityPropertiesString& operator=(const EntityPropertiesString &other) { if (&other != this) { EntityPropertiesBase::operator=(other); value =other.getValue(); }; return *this; };

	virtual eType getType(void) override { return STRING; };

	void setValue(const std::string &s) { if (value != s) setNeedsUpdate(); value = s; };
	const std::string& getValue(void) const { return value; };

	virtual bool hasSameValue(EntityPropertiesBase *other) override;

	virtual void addToJsonDocument(rapidjson::Document &jsonDoc, EntityBase *root) override;
	virtual void readFromJsonObject(const rapidjson::Value &object) override;

	virtual void copySettings(EntityPropertiesBase *other, EntityBase *root);

	static void createProperty(const std::string &group, const std::string &name, const std::string &defaultValue, const std::string &defaultCategory, EntityProperties &properties);

private:
	std::string value;
};

class __declspec(dllexport) EntityPropertiesSelection : public EntityPropertiesBase
{
public:
	EntityPropertiesSelection() {};
	virtual ~EntityPropertiesSelection() {};

	EntityPropertiesSelection(const EntityPropertiesSelection &other) : EntityPropertiesBase(other) { options = other.options; value = other.value; };

	virtual EntityPropertiesBase *createCopy(void) override { return new EntityPropertiesSelection(*this); };

	EntityPropertiesSelection& operator=(const EntityPropertiesSelection &other) { if (&other != this) { assert(checkCompatibilityOfSettings(other)); EntityPropertiesBase::operator=(other); value = other.getValue(); }; return *this; };

	virtual eType getType(void) override { return SELECTION; };

	virtual bool isCompatible(EntityPropertiesBase *other) override;

	bool setValue(const std::string &s);
	const std::string& getValue(void)const { return value; };

	void clearOptions() { options.clear(); };
	void addOption(const std::string &option) { assert(std::find(options.begin(), options.end(), option) == options.end());  options.push_back(option); }
	void resetOptions(std::list<std::string>& _options);
	const std::vector<std::string> &getOptions(void) { return options; };

	virtual bool hasSameValue(EntityPropertiesBase *other) override;

	virtual void addToJsonDocument(rapidjson::Document &jsonDoc, EntityBase *root) override;
	virtual void readFromJsonObject(const rapidjson::Value &object) override;

	virtual void copySettings(EntityPropertiesBase *other, EntityBase *root);

	static void createProperty(const std::string &group, const std::string &name, std::list<std::string>& options, const std::string &defaultValue, const std::string &defaultCategory, EntityProperties &properties);
	static void createProperty(const std::string &group, const std::string &name, std::list<std::string>&& options, const std::string &defaultValue, const std::string &defaultCategory, EntityProperties &properties);

private:
	bool checkCompatibilityOfSettings(const EntityPropertiesSelection &other);

	std::vector<std::string> options;
	std::string value;
};

class __declspec(dllexport) EntityPropertiesColor : public EntityPropertiesBase
{
public:
	EntityPropertiesColor() : color{ 0.0, 0.0, 0.0 } {};

	EntityPropertiesColor(const EntityPropertiesColor &other) : EntityPropertiesBase(other) { color[0] = other.getColorR(); color[1] = other.getColorG(); color[2] = other.getColorB(); };

	EntityPropertiesColor(const std::string &n, double r, double g, double b) : color{ r, g, b } { setName(n); };
	virtual ~EntityPropertiesColor() {};

	virtual EntityPropertiesBase *createCopy(void) override { return new EntityPropertiesColor(*this); };

	EntityPropertiesColor& operator=(const EntityPropertiesColor &other);

	virtual eType getType(void) override { return COLOR; };

	void setColorR(double c) { if (color[0] != c) setNeedsUpdate(); color[0] = c; };
	void setColorG(double c) { if (color[1] != c) setNeedsUpdate(); color[1] = c; };
	void setColorB(double c) { if (color[2] != c) setNeedsUpdate(); color[2] = c; };

	void setColorRGB(double r, double g, double b) { if (color[0] != r || color[1] != g || color[2] != b) setNeedsUpdate(); color[0] = r; color[1] = g; color[2] = b; };

	double getColorR(void) const { return color[0]; };
	double getColorG(void) const { return color[1]; };
	double getColorB(void) const { return color[2]; };

	virtual bool hasSameValue(EntityPropertiesBase *other) override;

	virtual void addToJsonDocument(rapidjson::Document &jsonDoc, EntityBase *root) override;
	virtual void readFromJsonObject(const rapidjson::Value &object) override;

	virtual void copySettings(EntityPropertiesBase *other, EntityBase *root);

	static void createProperty(const std::string &group, const std::string &name, std::vector<int> defaultValue, const std::string &defaultCategory, EntityProperties &properties);

private:
	double color[3];
};

class __declspec(dllexport) EntityPropertiesEntityList : public EntityPropertiesBase
{
public:
	EntityPropertiesEntityList() : entityContainerID(0), valueID(0) {};

	EntityPropertiesEntityList(const EntityPropertiesEntityList &other) : EntityPropertiesBase(other) { entityContainerName = other.getEntityContainerName(); entityContainerID = other.getEntityContainerID(); valueName = other.getValueName(); valueID = other.getValueID(); };
	EntityPropertiesEntityList(const std::string &n, const std::string &contName, ot::UID contID, const std::string &valName, ot::UID valID) : entityContainerName(contName), entityContainerID(contID), valueName(valName), valueID(valID) { setName(n); };

	virtual ~EntityPropertiesEntityList() {};

	virtual EntityPropertiesBase *createCopy(void) override { return new EntityPropertiesEntityList(*this); };

	EntityPropertiesEntityList& operator=(const EntityPropertiesEntityList &other);

	virtual eType getType(void) override { return ENTITYLIST; };

	virtual bool hasSameValue(EntityPropertiesBase *other) override;

	virtual void addToJsonDocument(rapidjson::Document &jsonDoc, EntityBase *root) override;
	virtual void readFromJsonObject(const rapidjson::Value &object) override;

	virtual void copySettings(EntityPropertiesBase *other, EntityBase *root);

	void setEntityContainerName(const std::string &containerName) { if (entityContainerName != containerName) setNeedsUpdate(); entityContainerName = containerName; };
	void setEntityContainerID(ot::UID containerID) { if (entityContainerID != containerID) setNeedsUpdate(); entityContainerID = containerID; };

	void setValueName(const std::string &vname) { if (valueName != vname) setNeedsUpdate(); valueName = vname; };
	void setValueID(ot::UID vid) { if (valueID != vid) setNeedsUpdate(); valueID = vid; };

	std::string getEntityContainerName(void) const { return entityContainerName; };
	ot::UID getEntityContainerID(void) const { return entityContainerID; };

	std::string getValueName(void) const { return valueName; };
	ot::UID getValueID(void) const { return valueID; };

	static void createProperty(const std::string &group, const std::string &name, const std::string &contName, ot::UID contID, const std::string &valName, ot::UID valID, const std::string &defaultCategory, EntityProperties &properties);

private:
	EntityContainer *findContainerFromID(EntityBase *root, ot::UID entityID);
	EntityContainer *findContainerFromName(EntityBase *root, const std::string &entityName);
	EntityBase *findEntityFromName(EntityBase *root, const std::string &entityName);
	EntityBase *findEntityFromID(EntityBase *root, ot::UID entityID);

	std::string entityContainerName;
	ot::UID entityContainerID;

	std::string valueName;
	ot::UID valueID;
};


class __declspec(dllexport) EntityPropertiesProjectList : public EntityPropertiesBase
{
public:
	EntityPropertiesProjectList() {};
	EntityPropertiesProjectList(const std::string& name) { setName(name); }
	virtual EntityPropertiesBase* createCopy(void) override { return new EntityPropertiesProjectList(*this); };
	virtual eType getType(void) override { return PROJECTLIST; };
	virtual void copySettings(EntityPropertiesBase* other, EntityBase* root);

	virtual bool hasSameValue(EntityPropertiesBase* other) override { return true; };
	virtual void addToJsonDocument(rapidjson::Document& jsonDoc, EntityBase* root) override;
	virtual void readFromJsonObject(const rapidjson::Value& object) override;

	void setValue(std::string& value) { _value = value; }
	std::string getValue() const {return _value;}
private:
	std::string _value;
};