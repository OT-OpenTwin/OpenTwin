#pragma once
#include "IVisualisationText.h"
#include "EntityBase.h"
class OT_MODELENTITIES_API_EXPORT EntityPythonManifest : public IVisualisationText, public EntityBase
{
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

	// Inherited via IVisualisationText
	std::string getText() override;
	void setText(const std::string& _text) override;
	ot::TextEditorCfg createConfig(bool _includeData) override;
	bool visualiseText() override { return true; }
	ot::UID getManifestID() const { return m_manifestID; }
	void setManifestID(ot::UID _id);

private:
	ot::UID m_manifestID = 0;
	std::string m_manifestText;

	virtual void addStorageData(bsoncxx::builder::basic::document& _storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view& _doc_view, std::map<ot::UID, EntityBase*>& _entityMap) override;
};
