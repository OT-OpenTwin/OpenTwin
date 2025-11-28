#pragma once
#include "IVisualisationText.h"
#include "EntityBase.h"
class __declspec(dllexport) EntityPythonManifest : public IVisualisationText, public EntityBase
{
	friend class FixturePythonManifest;
public:
	EntityPythonManifest() : EntityPythonManifest(0, nullptr, nullptr, nullptr) {};
	EntityPythonManifest(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms)
	: EntityBase(_ID,_parent,_obs,_ms){};

	virtual std::string getClassName(void) const override { return EntityPythonManifest::className(); };
	static std::string className() { return "EntityPythonManifest"; };

	// Inherited via EntityBase
	bool getEntityBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax) override { return false; }
	entityType getEntityType() const override { return TOPOLOGY; }
	virtual void addVisualizationNodes() override;
	void setEntityID(ot::UID id) override;
	// Inherited via IVisualisationText
	std::string getText() override;
	
	//! @brief Replaces the manifest text. If the environment has changed a new manifest ID is generated.
	void setText(const std::string& _text) override;
	ot::TextEditorCfg createConfig(const ot::VisualisationCfg& _visualizationConfig) override;
	bool visualiseText() override { return true; }
	
	ot::UID getManifestID() const { return m_manifestID; }
	void setManifestID(ot::UID _id);
	
	//! @brief  Creates a new manifest ID and returns it
	ot::UID generateNewManifestID();
	std::list<std::string> getManifestPackages() { return getPackageList(m_manifestText); }
	//! @brief Replaces manifest text without generating a new manifest ID
	void replaceManifest(const std::string& _newManifestText);

private:
	ot::UID m_manifestID = ot::invalidUID;
	std::string m_manifestText;

	bool environmentHasChanged(const std::string& _newContent);
	std::string trim(const std::string& _line);
	std::list<std::string> getPackageList(const std::string _text);

	virtual void addStorageData(bsoncxx::builder::basic::document& _storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view& _doc_view, std::map<ot::UID, EntityBase*>& _entityMap) override;
};
