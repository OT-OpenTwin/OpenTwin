#pragma once
#pragma warning(disable : 4251)

#include "EntityBase.h"
#include "TopoDS_Shape.hxx"
#include "TopoDS_TShape.hxx"
#include "TopoDS_Face.hxx"
#include "TopoDS_TFace.hxx"
#include "OldTreeIcon.h"

#include <list>
#include <map>

class __declspec(dllexport) EntityBrep : public EntityBase
{
public:
	EntityBrep() : EntityBrep(0, nullptr, nullptr, nullptr, "") {};
	EntityBrep(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, const std::string &owner);
	virtual ~EntityBrep();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	void		 setBrep(TopoDS_Shape &shape) { brep = shape; setModified(); }
	TopoDS_Shape &getBrep(void) { return brep; };

	static std::string className() { return "EntityBrep"; };
	virtual std::string getClassName(void) override { return EntityBrep::className(); };
	virtual entityType getEntityType(void) const override { return DATA; };

	gp_Trsf getTransform(void);
	void    setTransform(gp_Trsf transform);

	void setFaceName(const TopoDS_Face& face, const std::string &name);
	std::string getFaceName(const TopoDS_Face& face);

	void copyFaceNames(EntityBrep *other);
	void setFaceNameMap(std::map< const opencascade::handle<TopoDS_TShape>, std::string> &faceNames);
	std::map< const opencascade::handle<TopoDS_TShape>, std::string> getFaceNameMap();

private:
	virtual int getSchemaVersion(void) override { return 1; };
	virtual void addStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;

	TopoDS_Shape brep;

	gp_Trsf transformMatrix;

	std::map< const opencascade::handle<TopoDS_TShape>, std::string> m_faceNames;
};



