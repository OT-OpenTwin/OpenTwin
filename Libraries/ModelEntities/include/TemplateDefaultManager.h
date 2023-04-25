#pragma once
#pragma warning(disable : 4251)

#include <string>
#include <map>

class __declspec(dllexport) defaultValue
{
public:
	defaultValue() : type(UNDEFINED), d(0.0), l(0), b(false) {c[0] = c[1] = c[2] = 0; };
	defaultValue(const defaultValue &other);
	virtual ~defaultValue() {};

	defaultValue& operator=(const defaultValue &other);

	enum {UNDEFINED, DOUBLE, LONG, BOOL, STRING, COLOR} type;

	double		d;
	long		l;
	bool		b;
	std::string s;
	int			c[3];
};

class __declspec(dllexport) TemplateDefaultManager
{
public:
	TemplateDefaultManager();
	virtual ~TemplateDefaultManager();

	void loadDefaultTemplate(void);
	void setDefaultTemplate(const std::string &templateName);
	void loadDefaults(const std::string &category);
	std::string loadDefaultMaterials(void);

	double		getDefaultDouble(const std::string &category, const std::string &value, double defVal);
	long		getDefaultLong(const std::string &category, const std::string &value, long defVal);
	bool		getDefaultBool(const std::string &category, const std::string &value, bool defVal);
	std::string getDefaultString(const std::string &category, const std::string &value, const std::string &defVal);
	int			getDefaultColor(const std::string &category, const std::string &value, int component, int defVal);

	static TemplateDefaultManager* getTemplateDefaultManager(void);

	bool isUIMenuPageVisible(const std::string &page);
	bool isUIMenuGroupVisible(const std::string &page, const std::string &group);
	bool isUIMenuActionVisible(const std::string &page, const std::string &group, const std::string &action);

	const std::map < std::string, std::map<std::string, defaultValue>> &getDefaultMaterials(void) { return defaultMaterialsMap; }

private:
	void		 clearSettings(void);
	defaultValue getDefaultValue(const std::string &category, const std::string &value);

	std::string defaultTemplateName;
	std::map < std::string, std::map<std::string, defaultValue>> defaultMap;
	std::map < std::string, std::map<std::string, defaultValue>> defaultMaterialsMap;
};
