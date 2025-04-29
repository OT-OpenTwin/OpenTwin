#pragma once
#include "EntityBase.h"
#include "OTCore/Point2D.h"

class __declspec(dllexport) EntityCoordinates2D : public EntityBase
{
public:
	EntityCoordinates2D(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner);
	virtual bool getEntityBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax) override;
	virtual entityType getEntityType(void) const override { return DATA; };
	void setCoordinates(const ot::Point2DD& position) { m_location = position; setModified(); };
	const ot::Point2DD& getCoordinates() { return m_location; };
	virtual std::string getClassName(void) { return "EntityCoordinates2D"; };

	virtual std::string serialiseAsJSON() override;
	virtual bool deserialiseFromJSON(const ot::ConstJsonObject& _serialisation, ot::CopyInformation& _copyInformation, std::map<ot::UID, EntityBase*>& _entityMap) noexcept override;


private:
	
	ot::Point2DD m_location;

	void AddStorageData(bsoncxx::builder::basic::document& storage) override;
	void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;
};
