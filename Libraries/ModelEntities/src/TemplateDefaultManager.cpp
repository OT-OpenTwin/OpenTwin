
#include <cassert>

#include "TemplateDefaultManager.h"
#include "DataBase.h"

#include "Document\DocumentAccessBase.h"
#include "Document\DocumentAccess.h"
#include "Helper\QueryBuilder.h"

TemplateDefaultManager globalTemplateDefaultManager;

defaultValue::defaultValue(const defaultValue &other)
{
	type = other.type;

	d = other.d; 
	l = other.l; 
	b = other.b; 
	s = other.s; 
	c[0] = other.c[0]; c[1] = other.c[1]; c[2] = other.c[2];
}

defaultValue& defaultValue::operator=(const defaultValue &other)
{
	if (&other != this)
	{
		type = other.type;

		d = other.d;
		l = other.l;
		b = other.b;
		s = other.s;
		c[0] = other.c[0]; c[1] = other.c[1]; c[2] = other.c[2];
	}

	return *this;
}

TemplateDefaultManager* TemplateDefaultManager::getTemplateDefaultManager(void)
{
	return &globalTemplateDefaultManager;
}

TemplateDefaultManager::TemplateDefaultManager()
{

}

TemplateDefaultManager::~TemplateDefaultManager()
{

}

void TemplateDefaultManager::loadDefaultTemplate(void)
{
	clearSettings();
	defaultTemplateName.clear();

	// We are searching for the entity with schema type DefaultTemplate
	DataStorageAPI::DocumentAccessBase docBase("Projects", DataBase::GetDataBase()->getProjectName());

	auto queryDoc = bsoncxx::builder::basic::document{};
	queryDoc.append(bsoncxx::builder::basic::kvp("SchemaType", "DefaultTemplate"));

	auto filterDoc = bsoncxx::builder::basic::document{};

	auto result = docBase.GetDocument(std::move(queryDoc.extract()), std::move(filterDoc.extract()));
	if (!result) return; // DefaultTemplate not found

	// Now read the name of the default template from the item
	try
	{
		defaultTemplateName = result->view()["TemplateName"].get_utf8().value.data();
	}
	catch (std::exception)
	{
		assert(0); // unable to read the template name
	}
}

void TemplateDefaultManager::setDefaultTemplate(const std::string &templateName)
{
	if (defaultTemplateName != templateName)
	{
		clearSettings();
	}

	defaultTemplateName = templateName;
}

void TemplateDefaultManager::loadDefaults(const std::string &category)
{
	if (category.empty()) return; // No category was defined, so no default is specified.
	if (defaultTemplateName.empty()) return;  // No default template defined
	if (defaultMap.count(category) != 0) return; // This category was loaded already

	DataStorageAPI::DocumentAccessBase docBase("ProjectTemplates", defaultTemplateName);

	auto queryDoc = bsoncxx::builder::basic::document{};
	queryDoc.append(bsoncxx::builder::basic::kvp("Category", category));

	auto filterDoc = bsoncxx::builder::basic::document{};

	auto result = docBase.GetDocument(std::move(queryDoc.extract()), std::move(filterDoc.extract()));
	if (!result)
	{
		// The category was not found. We still create an empty entry such that we avoid trying to load the category in future
		std::map<std::string, defaultValue> categoryDefaults;
		defaultMap[category] = categoryDefaults;

		return; 
	}

	// We found the object with the default settings for the specified category -> read it and store it in the map
	std::map<std::string, defaultValue> categoryDefaults;

	for (auto elem : result->view()) 
	{
		defaultValue value;

		try
		{
			switch (elem.type())
			{
			case bsoncxx::type::k_bool:
				value.type = defaultValue::BOOL;
				value.b = elem.get_bool();
				break;
			case bsoncxx::type::k_double:
				value.type = defaultValue::DOUBLE;
				value.d = elem.get_double();
				break;
			case bsoncxx::type::k_utf8:
				value.type = defaultValue::STRING;
				value.s = elem.get_utf8().value.data();
				break;
			case bsoncxx::type::k_int32:
				value.type = defaultValue::LONG;
				value.l = elem.get_int32();
				break;
			case bsoncxx::type::k_int64:
				value.type = defaultValue::LONG;
				value.l = elem.get_int64();
				break;
			case bsoncxx::type::k_array:
			{
				auto values = elem.get_array().value;
				size_t numberValues = std::distance(values.begin(), values.end());

				auto pv = values.begin();

				if (numberValues == 3)
				{
					value.type = defaultValue::COLOR;  // We assume that this is a color entity

					for (unsigned long index = 0; index < numberValues; index++)
					{
						switch (pv->type())
						{
						case bsoncxx::type::k_int32:
							value.c[index] = pv->get_int32();
							break;
						case bsoncxx::type::k_int64:
							value.c[index] = pv->get_int64();
							break;
						default:
							value.type = defaultValue::UNDEFINED;  // The types do not match, so we don't have a color
						}

						pv++;
					}
				}

				break;
			}
			case bsoncxx::type::k_oid:
				// We can safely ignore this setting
				break;
			default:
				assert(0);  // Unknown type
			}

			if (value.type != defaultValue::UNDEFINED)
			{
				categoryDefaults[elem.key().to_string()] = value;
			}
		}
		catch (std::exception)
		{
			assert(0); // something went wrong
		}
	}

	// Remove the Category entry
	categoryDefaults.erase("Category");

	// Finally store the category in the global map
	defaultMap[category] = categoryDefaults;
}

double TemplateDefaultManager::getDefaultDouble(const std::string &category, const std::string &value, double defVal)
{
	defaultValue result = getDefaultValue(category, value);

	switch (result.type)
	{
	case defaultValue::DOUBLE:
		return result.d;
	case defaultValue::LONG:
		return (double) result.l;
	}	

	return defVal;
}

long TemplateDefaultManager::getDefaultLong(const std::string &category, const std::string &value, long defVal)
{
	defaultValue result = getDefaultValue(category, value);

	if (result.type == defaultValue::LONG) return result.l;

	return defVal;
}

bool TemplateDefaultManager::getDefaultBool(const std::string &category, const std::string &value, bool defVal)
{
	defaultValue result = getDefaultValue(category, value);

	if (result.type == defaultValue::BOOL) return result.b;

	return defVal;
}

std::string TemplateDefaultManager::getDefaultString(const std::string &category, const std::string &value, const std::string &defVal)
{
	defaultValue result = getDefaultValue(category, value);

	if (result.type == defaultValue::STRING) return result.s;

	return defVal;
}

int	TemplateDefaultManager::getDefaultColor(const std::string &category, const std::string &value, int component, int defVal)
{
	assert(component >= 0 && component < 3);

	defaultValue result = getDefaultValue(category, value);

	if (result.type == defaultValue::COLOR) return result.c[component];

	return defVal;
}

void TemplateDefaultManager::clearSettings(void)
{
	defaultMap.clear();
	defaultMaterialsMap.clear();
}

defaultValue TemplateDefaultManager::getDefaultValue(const std::string &category, const std::string &value)
{
	defaultValue result;

	if (defaultMap.count(category) == 0) return result;
	if (defaultMap[category].count(value) == 0) return result;

	return defaultMap[category][value];
}

bool TemplateDefaultManager::isUIMenuPageVisible(const std::string &page)
{
	defaultValue result = getDefaultValue("UI Configuration", page);

	if (result.type == defaultValue::BOOL) return result.b;

	// If no setting is defined, the page shall be visible
	return true; 
}

bool TemplateDefaultManager::isUIMenuGroupVisible(const std::string &page, const std::string &group)
{
	// Check whether menu page is visible at all
	if (!isUIMenuPageVisible(page)) return false;

	// Now check whether the specified group is visible
	defaultValue result = getDefaultValue("UI Configuration", page + ":" + group);

	if (result.type == defaultValue::BOOL) return result.b;

	// If no setting is defined, the group shall be visible
	return true; 
}

bool TemplateDefaultManager::isUIMenuActionVisible(const std::string &page, const std::string &group, const std::string &action)
{
	// Check whether menu page is visible at all
	if (!isUIMenuPageVisible(page)) return false;

	// Check whether menu group is visible at all
	if (!isUIMenuGroupVisible(page, group)) return false;

	// Now check whether the specified action is visible
	defaultValue result = getDefaultValue("UI Configuration", page + ":" + group + ":" + action);

	if (result.type == defaultValue::BOOL) return result.b;

	// If no setting is defined, the action shall be visible
	return true; 
}

std::string TemplateDefaultManager::loadDefaultMaterials(void)
{
	defaultMaterialsMap.clear();
	if (defaultTemplateName.empty()) return "";  // No default template defined

	std::string error;

	DataStorageAPI::DocumentAccessBase docBase("ProjectTemplates", defaultTemplateName);

	auto queryDoc = bsoncxx::builder::basic::document{};
	queryDoc.append(bsoncxx::builder::basic::kvp("Category", "DefaultMaterial"));

	auto filterDoc = bsoncxx::builder::basic::document{};

	auto results = docBase.GetAllDocument(std::move(queryDoc.extract()), std::move(filterDoc.extract()), 0);

	for (auto result : results)
	{
		std::string materialName;

		try
		{
			// Now we iterate though the found documents where each document corresponds to a default material
			materialName = result["Name"].get_utf8().value.data();

			std::map<std::string, defaultValue> materialDefaults;

			for (auto elem : result)
			{
				if (elem.key() == "Category") continue;
				if (elem.key() == "Name") continue;

				defaultValue value;

				try
				{
					switch (elem.type())
					{
					case bsoncxx::type::k_bool:
						value.type = defaultValue::BOOL;
						value.b = elem.get_bool();
						break;
					case bsoncxx::type::k_double:
						value.type = defaultValue::DOUBLE;
						value.d = elem.get_double();
						break;
					case bsoncxx::type::k_utf8:
						value.type = defaultValue::STRING;
						value.s = elem.get_utf8().value.data();
						break;
					case bsoncxx::type::k_int32:
						value.type = defaultValue::LONG;
						value.l = elem.get_int32();
						break;
					case bsoncxx::type::k_int64:
						value.type = defaultValue::LONG;
						value.l = elem.get_int64();
						break;
					case bsoncxx::type::k_array:
					{
						auto values = elem.get_array().value;
						size_t numberValues = std::distance(values.begin(), values.end());

						auto pv = values.begin();

						if (numberValues == 3)
						{
							value.type = defaultValue::COLOR;  // We assume that this is a color entity

							for (unsigned long index = 0; index < numberValues; index++)
							{
								switch (pv->type())
								{
								case bsoncxx::type::k_int32:
									value.c[index] = pv->get_int32();
									break;
								case bsoncxx::type::k_int64:
									value.c[index] = pv->get_int64();
									break;
								default:
									value.type = defaultValue::UNDEFINED;  // The types do not match, so we don't have a color
								}

								pv++;
							}
						}

						break;
					}
					case bsoncxx::type::k_oid:
						// We can safely ignore this setting
						break;
					default:
						assert(0);  // Unknown type
					}

					if (value.type != defaultValue::UNDEFINED)
					{
						materialDefaults[elem.key().to_string()] = value;
					}
				}
				catch (std::exception)
				{
					assert(0); // something went wrong
				}
			}

			defaultMaterialsMap[materialName] = materialDefaults;
		}
		catch (std::exception)
		{
			if (materialName.empty())
			{
				error += "ERROR: Unable to load default material from template (name missing)\n";
			}
			else
			{
				error += "ERROR: Unable to load default material from template (" + materialName + ")\n";
			}
		}
	}

	return error;
}
