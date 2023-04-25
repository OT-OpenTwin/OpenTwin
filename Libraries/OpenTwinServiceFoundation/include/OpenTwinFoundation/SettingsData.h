#pragma once
#pragma warning(disable : 4251)

// OpenTwin header
#include "OpenTwinCore/Color.h"
#include "OpenTwinCore/rJSON.h"
#include "OpenTwinFoundation/FoundationAPIExport.h"
#include "OpenTwinFoundation/UserCredentials.h"

// C++ header
#include <string>
#include <list>

namespace ot {

	class SettingsGroup;
	class AbstractSettingsItem;

	class OT_SERVICEFOUNDATION_API_EXPORT SettingsData {
	public:
		//! @brief Constructor
		//! @param
		SettingsData(const std::string& _settingsDataName, const std::string& _settingsVersion);

		virtual ~SettingsData();

		static SettingsData * parseFromJsonDocument(rapidjson::Document& _document);

		static SettingsData * parseFromJsonObject(rapidjson::Value& _object);

		static SettingsGroup * parseGroupFromJsonObject(rapidjson::Value& _object);

		// ##############################################################

		// Getter

		inline const std::string& dataName(void) const { return m_settingsDataName; }

		inline const std::string& version(void) const { return m_settingsVersion; }

		inline 	const std::list<SettingsGroup *>& groups(void) const { return m_groups; }

		static void splitString(std::list<std::string>& _list, const std::string& _string, const std::string& _delimiter);

		std::string toJson(void) const;

		void addToJsonDocument(rapidjson::Document& _document) const;

		std::list<AbstractSettingsItem *> items(void);

		std::list<std::string> itemNames(void);

		SettingsGroup * findGroupByLogicalName(const std::string& _logicalName, const std::string& _delimiter = std::string(":"));

		AbstractSettingsItem * findItemByLogicalName(const std::string& _logicalName, const std::string& _delimiter = std::string(":"));

		// ##############################################################

		// Setter

		SettingsGroup* addGroup(const std::string& _name, const std::string& _title);

		void addGroup(SettingsGroup * _group);

		bool saveToDatabase(const std::string &databaseURL, const std::string &siteID, const std::string &userName, const std::string &userPassword, const std::string& _userCollection);

		bool refreshValuesFromDatabase(const std::string &databaseURL, const std::string &siteID, const std::string &userName, const std::string &userPassword, const std::string& _userCollection);

	protected:

		std::string						m_settingsDataName;
		std::string						m_settingsVersion;
		std::list<SettingsGroup *>		m_groups;
	};

	// ############################################################################################

	// ############################################################################################

	// ############################################################################################

	class OT_SERVICEFOUNDATION_API_EXPORT SettingsGroup {
	public:
		SettingsGroup(const std::string& _name, const std::string& _title);

		virtual ~SettingsGroup();

		// ##############################################################

		// Setter

		void addSubgroup(SettingsGroup * _group);

		SettingsGroup * addSubgroup(const std::string& _name, const std::string& _title);

		void addItem(AbstractSettingsItem * _item);

		// ##############################################################

		// Getter

		inline const std::list<SettingsGroup *>& subgroups(void) const { return m_subgroups; }

		inline const std::list<AbstractSettingsItem *>& items(void) const { return m_items; }

		inline const std::string& name(void) const { return m_name; }

		inline const std::string& title(void) const { return m_title; }

		inline SettingsGroup * parentGroup(void) { return m_parent; }

		inline bool isAttached(void) const { return m_isAttached; }

		void addToJsonArray(rapidjson::Document& _document, rapidjson::Value& _array) const;

		std::string logicalName(const std::string& _delimiter = std::string(":"));

		SettingsGroup * findGroupByLogicalName(std::list<std::string>& _logicalName);

		AbstractSettingsItem * findItemByLogicalName(std::list<std::string>& _logicalName);

	private:
		friend SettingsData;
		void setAttached(void) { m_isAttached = true; }
		void setParent(SettingsGroup * _parentGroup) { m_parent = _parentGroup; m_isAttached = true; }

		SettingsGroup *						m_parent;
		std::string							m_name;
		std::string							m_title;
		bool								m_isAttached;

		std::list<SettingsGroup *>			m_subgroups;
		std::list<AbstractSettingsItem *>	m_items;

		SettingsGroup() = delete;
		SettingsGroup(SettingsGroup&) = delete;
		SettingsGroup& operator = (SettingsGroup&) = delete;
	};

	// ############################################################################################

	// ############################################################################################

	// ############################################################################################

	class OT_SERVICEFOUNDATION_API_EXPORT AbstractSettingsItem {
	public:
		enum itemType {
			InfoText,
			String,
			Integer,
			Double,
			Boolean,
			Selection,
			EditableSelection,
			ListSelection,
			Color,
			DirectorySelect,
			FileSelectSave,
			FileSelectOpen
		};

		AbstractSettingsItem(itemType _type, const std::string& _name, const std::string& _title, const std::string& _description = std::string());

		virtual ~AbstractSettingsItem();

		virtual AbstractSettingsItem * createCopy(void) const = 0;

		virtual void copyValuesFromOtherItem(AbstractSettingsItem * _item) = 0;

		inline void setVisible(bool _isVisible = true) { m_isVisible = _isVisible; }

		inline void setHidden(void) { m_isVisible = false; }

		inline void setTitle(const std::string& _title) { m_title = _title; }

		inline void setDescription(const std::string& _description) { m_description = _description; }

		// ##############################################################

		// Getter

		inline itemType type(void) const { return m_type; }

		inline const std::string& name(void) const { return m_name; }

		inline const std::string& title(void) const { return m_title; }

		inline const std::string& description(void) const { return m_description; }

		inline SettingsGroup * parentGroup(void) { return m_parent; }

		inline bool isVisible(void) const { return m_isVisible; }

		std::string typeToString(void) const;

		static std::string typeToString(itemType _type);

		std::list<std::string> parentGroupsList(void) const;

		void addToJsonArray(rapidjson::Document& _document, rapidjson::Value& _array) const;

		std::string logicalName(const std::string& _delimiter = std::string(":"));

	protected:

		virtual void addItemDataToJsonObject(rapidjson::Document& _document, rapidjson::Value& _object) const = 0;

		SettingsGroup *	m_parent;
		itemType		m_type;
		std::string		m_name;
		std::string		m_description;
		std::string		m_title;

	private:
		friend SettingsGroup;
		void setParent(SettingsGroup * _parentGroup) { m_parent = _parentGroup; }
		bool isAttached(void) const { return m_parent; }
		bool			m_isVisible;

	private:
		AbstractSettingsItem();
		AbstractSettingsItem(AbstractSettingsItem&) = delete;
		AbstractSettingsItem& operator = (AbstractSettingsItem&) = delete;
	};

	// ############################################################################################

	// ############################################################################################

	// ############################################################################################

	class OT_SERVICEFOUNDATION_API_EXPORT SettingsItemInfoText : public AbstractSettingsItem {
	public:
		explicit SettingsItemInfoText(const std::string& _name, const std::string& _title, const std::string& _description = std::string());

		virtual ~SettingsItemInfoText();

		virtual AbstractSettingsItem * createCopy(void) const override;

		virtual void copyValuesFromOtherItem(AbstractSettingsItem * _item) override;

		inline void setText(const std::string& _text);

		// ##############################################################

		// Getter

		inline std::string value(void) const;

	protected:
		virtual void addItemDataToJsonObject(rapidjson::Document& _document, rapidjson::Value& _object) const override;

	private:
		SettingsItemInfoText() = delete;
		SettingsItemInfoText(SettingsItemInfoText&) = delete;
		SettingsItemInfoText& operator = (SettingsItemInfoText&) = delete;
	};

	// ############################################################################################

	// ############################################################################################

	// ############################################################################################

	class OT_SERVICEFOUNDATION_API_EXPORT SettingsItemBoolean : public AbstractSettingsItem {
	public:
		explicit SettingsItemBoolean(const std::string& _name, const std::string& _title, bool _value = false, const std::string& _description = std::string());

		virtual ~SettingsItemBoolean();

		virtual AbstractSettingsItem * createCopy(void) const override;

		virtual void copyValuesFromOtherItem(AbstractSettingsItem * _item) override;

		inline void setValue(bool _value) { m_value = _value; }

		// ##############################################################

		// Getter

		inline bool value(void) const { return m_value; }

	protected:
		virtual void addItemDataToJsonObject(rapidjson::Document& _document, rapidjson::Value& _object) const override;

	private:
		bool			m_value;

		SettingsItemBoolean() = delete;
		SettingsItemBoolean(SettingsItemBoolean&) = delete;
		SettingsItemBoolean& operator = (SettingsItemBoolean&) = delete;
	};

	// ############################################################################################

	// ############################################################################################

	// ############################################################################################

	class OT_SERVICEFOUNDATION_API_EXPORT SettingsItemString : public AbstractSettingsItem {
	public:
		explicit SettingsItemString(const std::string& _name, const std::string& _title, const std::string& _value = std::string(), const std::string& _inputHint = std::string(), const std::string& _description = std::string());
	
		virtual ~SettingsItemString();

		virtual AbstractSettingsItem * createCopy(void) const override;

		virtual void copyValuesFromOtherItem(AbstractSettingsItem * _item) override;

		inline void setValue(const std::string& _value) { m_value = _value; }

		inline void setInputHint(const std::string& _inputHint) { m_inputHint = _inputHint; }

		// ##############################################################

		// Getter

		inline const std::string& value(void) const { return m_value; }

		inline const std::string& inputHint(void) const { return m_inputHint; }

	protected:
		virtual void addItemDataToJsonObject(rapidjson::Document& _document, rapidjson::Value& _object) const override;

	private:
		std::string			m_value;
		std::string			m_inputHint;
		
		SettingsItemString() = delete;
		SettingsItemString(SettingsItemString&) = delete;
		SettingsItemString& operator = (SettingsItemString&) = delete;
	};

	// ############################################################################################

	// ############################################################################################

	// ############################################################################################

	class OT_SERVICEFOUNDATION_API_EXPORT SettingsItemInteger : public AbstractSettingsItem {
	public:
		explicit SettingsItemInteger(const std::string& _name, const std::string& _title, int _value = 0, int _minValue = 0, int _maxValue = 100, const std::string& _description = std::string());

		virtual ~SettingsItemInteger();

		virtual AbstractSettingsItem * createCopy(void) const override;

		virtual void copyValuesFromOtherItem(AbstractSettingsItem * _item) override;

		inline void setValue(int _value);

		inline void setMinValue(int _min);

		inline void setMaxValue(int _max);

		inline void setLimits(int _min, int _max);

		inline void setMaxLimits(void);

		// ##############################################################

		// Getter

		inline int value(void) const { return m_value; }

		inline int minValue(void) const { return m_minValue; }

		inline int maxValue(void) const { return m_maxValue; }

	protected:
		virtual void addItemDataToJsonObject(rapidjson::Document& _document, rapidjson::Value& _object) const override;

	private:

		void checkLimits(void);

		int				m_value;
		int				m_minValue;
		int				m_maxValue;

		SettingsItemInteger() = delete;
		SettingsItemInteger(SettingsItemInteger&) = delete;
		SettingsItemInteger& operator = (SettingsItemInteger&) = delete;
	};

	// ############################################################################################

	// ############################################################################################

	// ############################################################################################

	class OT_SERVICEFOUNDATION_API_EXPORT SettingsItemDouble : public AbstractSettingsItem {
	public:
		explicit SettingsItemDouble(const std::string& _name, const std::string& _title, double _value = 0., double _minValue = 0., double _maxValue = 100., int _decimals = 2, const std::string& _description = std::string());

		virtual ~SettingsItemDouble();

		virtual AbstractSettingsItem * createCopy(void) const override;

		virtual void copyValuesFromOtherItem(AbstractSettingsItem * _item) override;

		inline 	void setValue(int _value);

		inline void setMinValue(int _min);

		inline void setMaxValue(int _max);

		inline void setLimits(int _min, int _max);

		inline void setMaxLimits(void);

		// ##############################################################

		// Getter

		inline double value(void) const { return m_value; }

		inline double minValue(void) const { return m_minValue; }

		inline double maxValue(void) const { return m_maxValue; }

		inline int decimalPlaces(void) const { return m_decimals; }

	protected:
		virtual void addItemDataToJsonObject(rapidjson::Document& _document, rapidjson::Value& _object) const override;

	private:

		void checkLimits(void);

		double			m_value;
		double			m_minValue;
		double			m_maxValue;
		int				m_decimals;

		SettingsItemDouble() = delete;
		SettingsItemDouble(SettingsItemDouble&) = delete;
		SettingsItemDouble& operator = (SettingsItemDouble&) = delete;
	};

	// ############################################################################################

	// ############################################################################################

	// ############################################################################################

	class OT_SERVICEFOUNDATION_API_EXPORT SettingsItemSelection : public AbstractSettingsItem {
	public:

		explicit SettingsItemSelection(const std::string& _name, const std::string& _title, const std::list<std::string>& _possibleSelection = std::list<std::string>(), const std::string& _selectedValue = std::string(), const std::string& _description = std::string());

		virtual ~SettingsItemSelection();

		virtual AbstractSettingsItem * createCopy(void) const override;

		virtual void copyValuesFromOtherItem(AbstractSettingsItem * _item) override;

		inline void addPossibleSelection(const std::string& _possibleSelection);

		inline void addPossibleSelection(const std::list<std::string>&  _possibleSelection);

		inline void setPossibleSelection(const std::list<std::string>& _possibleSelection);

		inline void setSelectedValue(const std::string& _selectedValue);

		// ##############################################################

		// Getter

		inline const std::list<std::string>& possibleSelection(void) const { return m_possibleSelection; }

		inline const std::string& selectedValue(void) const { return m_selectedValue; }

	protected:
		virtual void addItemDataToJsonObject(rapidjson::Document& _document, rapidjson::Value& _object) const override;

	private:
		std::list<std::string>	m_possibleSelection;
		std::string				m_selectedValue;

		SettingsItemSelection() = delete;
		SettingsItemSelection(SettingsItemSelection&) = delete;
		SettingsItemSelection& operator = (SettingsItemSelection&) = delete;
	};

	// ############################################################################################

	// ############################################################################################

	// ############################################################################################

	class OT_SERVICEFOUNDATION_API_EXPORT SettingsItemEditableSelection : public AbstractSettingsItem {
	public:

		explicit SettingsItemEditableSelection(const std::string& _name, const std::string& _title, const std::list<std::string>& _possibleSelection = std::list<std::string>(), const std::string& _selectedValue = std::string(), const std::string& _inputHint = std::string(), const std::string& _description = std::string());

		virtual ~SettingsItemEditableSelection();

		virtual AbstractSettingsItem * createCopy(void) const override;

		virtual void copyValuesFromOtherItem(AbstractSettingsItem * _item) override;

		inline void setInputHint(const std::string& _hint) { m_inputHint = _hint; }

		inline void addPossibleSelection(const std::string& _possibleSelection);

		inline void addPossibleSelection(const std::list<std::string>&  _possibleSelection);

		inline void setPossibleSelection(const std::list<std::string>& _possibleSelection);

		inline void setSelectedValue(const std::string& _selectedValue);

		// ##############################################################

		// Getter

		inline const std::list<std::string>& possibleSelection(void) const { return m_possibleSelection; }

		inline const std::string& selectedValue(void) const { return m_selectedValue; }

		inline const std::string& inputHint(void) const { return m_inputHint; }

	protected:
		virtual void addItemDataToJsonObject(rapidjson::Document& _document, rapidjson::Value& _object) const override;

	private:
		std::list<std::string>	m_possibleSelection;
		std::string				m_selectedValue;
		std::string				m_inputHint;

		SettingsItemEditableSelection() = delete;
		SettingsItemEditableSelection(SettingsItemEditableSelection&) = delete;
		SettingsItemEditableSelection& operator = (SettingsItemEditableSelection&) = delete;
	};

	// ############################################################################################

	// ############################################################################################

	// ############################################################################################

	class OT_SERVICEFOUNDATION_API_EXPORT SettingsItemListSelection : public AbstractSettingsItem {
	public:

		explicit SettingsItemListSelection(const std::string& _name, const std::string& _title, const std::list<std::string>& _possibleSelection = std::list<std::string>(), const std::list<std::string>& _selectedValues = std::list<std::string>(), const std::string& _description = std::string());

		virtual ~SettingsItemListSelection();

		virtual AbstractSettingsItem * createCopy(void) const override;

		virtual void copyValuesFromOtherItem(AbstractSettingsItem * _item) override;

		inline void addPossibleSelection(const std::string& _possibleSelection);

		inline void addPossibleSelection(const std::list<std::string>&  _possibleSelection);

		inline void setPossibleSelection(const std::list<std::string>& _possibleSelection);

		inline void addSelectedValue(const std::string& _selectedValue);

		inline void addSelectedValues(const std::list<std::string>& _selectedValues);

		inline void setSelectedValues(const std::list<std::string>& _selectedValues);

		// ##############################################################

		// Getter

		inline const std::list<std::string>& possibleSelection(void) const { return m_possibleSelection; }

		inline const std::list<std::string>& selectedValues(void) const { return m_selectedValues; }

	protected:
		virtual void addItemDataToJsonObject(rapidjson::Document& _document, rapidjson::Value& _object) const override;

	private:
		std::list<std::string>	m_possibleSelection;
		std::list<std::string>	m_selectedValues;

		SettingsItemListSelection() = delete;
		SettingsItemListSelection(SettingsItemListSelection&) = delete;
		SettingsItemListSelection& operator = (SettingsItemListSelection&) = delete;
	};

	// ############################################################################################

	// ############################################################################################

	// ############################################################################################

	class OT_SERVICEFOUNDATION_API_EXPORT SettingsItemColor : public AbstractSettingsItem {
	public:

		explicit SettingsItemColor(const std::string& _name, const std::string& _title, const ot::Color& _color, const std::string& _description = std::string());

		virtual ~SettingsItemColor();

		virtual AbstractSettingsItem * createCopy(void) const override;

		virtual void copyValuesFromOtherItem(AbstractSettingsItem * _item) override;

		inline void setValue(const ot::Color& _color) { m_color = _color; }

		inline void setValue(int _r, int _g, int _b, int _a = 255);

		inline void setValue(float _r, float _g, float _b, float _a = 1.f);

		// ##############################################################

		// Getter

		inline const ot::Color& value(void) const { return m_color; }

	protected:
		virtual void addItemDataToJsonObject(rapidjson::Document& _document, rapidjson::Value& _object) const override;

	private:
		ot::Color		m_color;

		SettingsItemColor() = delete;
		SettingsItemColor(SettingsItemColor&) = delete;
		SettingsItemColor& operator = (SettingsItemColor&) = delete;
	};

	// ############################################################################################

	// ############################################################################################

	// ############################################################################################

	class OT_SERVICEFOUNDATION_API_EXPORT SettingsItemDirectorySelect : public AbstractSettingsItem {
	public:

		explicit SettingsItemDirectorySelect(const std::string& _name, const std::string& _title, const std::string& _directory, const std::string& _inputHint = std::string(), const std::string& _description = std::string());

		virtual ~SettingsItemDirectorySelect();

		virtual AbstractSettingsItem * createCopy(void) const override;

		virtual void copyValuesFromOtherItem(AbstractSettingsItem * _item) override;

		inline 	void setDirectory(const std::string& _directory) { m_directory = _directory; }

		inline void setInputHint(const std::string& _hint) { m_inputHint = _hint; }

		// ##############################################################

		// Getter

		inline const std::string& directory(void) const { return m_directory; }

		inline const std::string& inputHint(void) const { return m_inputHint; }

	protected:
		virtual void addItemDataToJsonObject(rapidjson::Document& _document, rapidjson::Value& _object) const override;

	private:
		std::string			m_directory;
		std::string			m_inputHint;

		SettingsItemDirectorySelect() = delete;
		SettingsItemDirectorySelect(SettingsItemDirectorySelect&) = delete;
		SettingsItemDirectorySelect& operator = (SettingsItemDirectorySelect&) = delete;
	};

	// ############################################################################################

	// ############################################################################################

	// ############################################################################################

	class OT_SERVICEFOUNDATION_API_EXPORT SettingsItemFileSelectOpen : public AbstractSettingsItem {
	public:

		explicit SettingsItemFileSelectOpen(const std::string& _name, const std::string& _title, const std::string& _file, const std::string& _filter = std::string(), const std::string& _inputHint = std::string(), const std::string& _description = std::string());

		virtual ~SettingsItemFileSelectOpen();

		virtual AbstractSettingsItem * createCopy(void) const override;

		virtual void copyValuesFromOtherItem(AbstractSettingsItem * _item) override;

		inline void setFile(const std::string& _file) { m_file = _file; }

		inline void setFilter(const std::string& _filter) { m_filter = _filter; }

		inline void setInputHint(const std::string& _hint) { m_inputHint = _hint; }

		// ##############################################################

		// Getter

		inline const std::string& file(void) const { return m_file; }

		inline const std::string& filter(void) const { return m_filter; }

		inline const std::string& inputHint(void) const { return m_inputHint; }

	protected:
		virtual void addItemDataToJsonObject(rapidjson::Document& _document, rapidjson::Value& _object) const override;

	private:
		std::string			m_file;
		std::string			m_filter;
		std::string			m_inputHint;

		SettingsItemFileSelectOpen() = delete;
		SettingsItemFileSelectOpen(SettingsItemFileSelectOpen&) = delete;
		SettingsItemFileSelectOpen& operator = (SettingsItemFileSelectOpen&) = delete;
	};

	// ############################################################################################

	// ############################################################################################

	// ############################################################################################

	class OT_SERVICEFOUNDATION_API_EXPORT SettingsItemFileSelectSave : public AbstractSettingsItem {
	public:

		explicit SettingsItemFileSelectSave(const std::string& _name, const std::string& _title, const std::string& _file, const std::string& _filter = std::string(), const std::string& _inputHint = std::string(), const std::string& _description = std::string());

		virtual ~SettingsItemFileSelectSave();

		virtual AbstractSettingsItem * createCopy(void) const override;

		virtual void copyValuesFromOtherItem(AbstractSettingsItem * _item) override;

		inline void setFile(const std::string& _file) { m_file = _file; }

		inline void setFilter(const std::string& _filter) { m_filter = _filter; }

		inline void setInputHint(const std::string& _hint) { m_inputHint = _hint; }

		// ##############################################################

		// Getter

		inline const std::string& file(void) const { return m_file; }

		inline const std::string& filter(void) const { return m_filter; }

		inline const std::string& inputHint(void) const { return m_inputHint; }

	protected:
		virtual void addItemDataToJsonObject(rapidjson::Document& _document, rapidjson::Value& _object) const override;

	private:
		std::string			m_file;
		std::string			m_filter;
		std::string			m_inputHint;

		SettingsItemFileSelectSave() = delete;
		SettingsItemFileSelectSave(SettingsItemFileSelectSave&) = delete;
		SettingsItemFileSelectSave& operator = (SettingsItemFileSelectSave&) = delete;
	};

	// ############################################################################################

	// ############################################################################################

	// ############################################################################################

	class OT_SERVICEFOUNDATION_API_EXPORT SettingsItemFactory {
	public:
		static AbstractSettingsItem * createItem(rapidjson::Value& _jsonObject);

		static SettingsItemInfoText * createInfoTextItem(const std::string& _name, const std::string& _title, const std::string& _description, bool _isVisible = true);

		static SettingsItemBoolean * createBooleanItem(const std::string& _name, const std::string& _title, const std::string& _description, bool _value);
		static SettingsItemBoolean * createBooleanItem(const std::string& _name, const std::string& _title, const std::string& _description, bool _isVisible, rapidjson::Value& _jsonObject);

		static SettingsItemString * createStringItem(const std::string& _name, const std::string& _title, const std::string& _description, const std::string& _value, const std::string& _inputHint = std::string());
		static SettingsItemString * createStringItem(const std::string& _name, const std::string& _title, const std::string& _description, bool _isVisible, rapidjson::Value& _jsonObject);

		static SettingsItemInteger * createIntegerItem(const std::string& _name, const std::string& _title, const std::string& _description, int _value, int _minValue, int _maxValue);
		static SettingsItemInteger * createIntegerItem(const std::string& _name, const std::string& _title, const std::string& _description, bool _isVisible, rapidjson::Value& _jsonObject);

		static SettingsItemDouble * createDoubleItem(const std::string& _name, const std::string& _title, const std::string& _description, double _value, double _minValue, double _maxValue, int _decimals);
		static SettingsItemDouble * createDoubleItem(const std::string& _name, const std::string& _title, const std::string& _description, bool _isVisible, rapidjson::Value& _jsonObject);

		static SettingsItemSelection * createSelectionItem(const std::string& _name, const std::string& _title, const std::string& _description, const std::list<std::string>& _possibleSelection = std::list<std::string>(), const std::string& _selectedValue = std::string());
		static SettingsItemSelection * createSelectionItem(const std::string& _name, const std::string& _title, const std::string& _description, bool _isVisible, rapidjson::Value& _jsonObject);

		static SettingsItemEditableSelection * createEditableSelectionItem(const std::string& _name, const std::string& _title, const std::string& _description, const std::list<std::string>& _possibleSelection = std::list<std::string>(), const std::string& _selectedValue = std::string(), const std::string& _inputHint = std::string());
		static SettingsItemEditableSelection * createEditableSelectionItem(const std::string& _name, const std::string& _title, const std::string& _description, bool _isVisible, rapidjson::Value& _jsonObject);

		static SettingsItemListSelection * createListSelectionItem(const std::string& _name, const std::string& _title, const std::string& _description, const std::list<std::string>& _possibleSelection = std::list<std::string>(), const std::list<std::string>& _selectedValues = std::list<std::string>());
		static SettingsItemListSelection * createListSelectionItem(const std::string& _name, const std::string& _title, const std::string& _description, bool _isVisible, rapidjson::Value& _jsonObject);

		static SettingsItemColor * createColorItem(const std::string& _name, const std::string& _title, const std::string& _description, const ot::Color& _color);
		static SettingsItemColor * createColorItem(const std::string& _name, const std::string& _title, const std::string& _description, bool _isVisible, rapidjson::Value& _jsonObject);

		static SettingsItemDirectorySelect * createDirectorySelectItem(const std::string& _name, const std::string& _title, const std::string& _description, const std::string& _directory, const std::string& _inputHint = std::string());
		static SettingsItemDirectorySelect * createDirectorySelectItem(const std::string& _name, const std::string& _title, const std::string& _description, bool _isVisible, rapidjson::Value& _jsonObject);

		static SettingsItemFileSelectOpen * createFileSelectOpenItem(const std::string& _name, const std::string& _title, const std::string& _description, const std::string& _file, const std::string& _inputHint = std::string());
		static SettingsItemFileSelectOpen * createFileSelectOpenItem(const std::string& _name, const std::string& _title, const std::string& _description, bool _isVisible, rapidjson::Value& _jsonObject);

		static SettingsItemFileSelectSave * createFileSelectSaveItem(const std::string& _name, const std::string& _title, const std::string& _description, const std::string& _file, const std::string& _inputHint = std::string());
		static SettingsItemFileSelectSave * createFileSelectSaveItem(const std::string& _name, const std::string& _title, const std::string& _description, bool _isVisible, rapidjson::Value& _jsonObject);

	private:
		SettingsItemFactory() = delete;
		SettingsItemFactory(SettingsItemFactory&) = delete;
		SettingsItemFactory & operator = (SettingsItemFactory&) = delete;
	};
}