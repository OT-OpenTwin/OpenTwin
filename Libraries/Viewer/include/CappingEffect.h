#pragma once

#include <osgFX/Effect>
#include <osg/Geometry>
#include <osg/Stencil>
#include <osg/Depth>
#include <osg/Projection>
#include <osg/PolygonMode>

class CappingTechnique : public osgFX::Technique {

private:
	osg::ref_ptr<osg::Node> _capPlane;

public:
	virtual bool validate(osg::State&) const
	{ return true; }

	META_Technique("CappingTechnique","Pimpel capping technique");
	/// Constructor
	CappingTechnique()
	{
		// Build the plane to draw with the stencil mask
		osg::Geometry *geometry = new osg::Geometry();
		osg::Vec3Array *vertices = new osg::Vec3Array();
		osg::Vec3Array *normals = new osg::Vec3Array();
		osg::Vec4Array *colors = new osg::Vec4Array();

		vertices->push_back(osg::Vec3(-10.0, -10.0, 0.1));
		vertices->push_back(osg::Vec3(-10.0, 10.0, 0.1));
		vertices->push_back(osg::Vec3(10.0, 10.0, 0.1));
		vertices->push_back(osg::Vec3(10.0, -10.0, 0.1));
		normals->push_back(osg::Vec3(0.0, 0.0, 1.0));
		geometry->setVertexArray(vertices);
		geometry->setNormalArray(normals);
		colors->push_back(osg::Vec4(1.0, 0.0, 1.0, 1.0));
		geometry->setColorArray(colors);
		geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
		geometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
		geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));

		osg::Geode *geode = new osg::Geode();
		geode->addDrawable(geometry);
		geode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

		_capPlane = geode;
		return;

		osg::Transform *trans = new osg::Transform();
		trans->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
		trans->addChild( geode );

		osg::Projection *proj = new osg::Projection(osg::Matrix::ortho2D(-1,1,-1,1));
		proj->addChild( trans );

		_capPlane = proj;

	}
protected:
	virtual void define_passes()
	{
		// pass #0
		{
			osg::ref_ptr<osg::StateSet> ss = new osg::StateSet;
			osg::Stencil *stencil = new osg::Stencil;
			stencil->setFunction(osg::Stencil::ALWAYS, 0x0, ~0);
			stencil->setOperation(osg::Stencil::INVERT, osg::Stencil::INVERT, osg::Stencil::INVERT);
			ss->setAttributeAndModes(stencil, osg::StateAttribute::ON); // | osg::StateAttribute::OVERRIDE);
			ss->setMode(GL_CULL_FACE,osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
			addPass(ss.get());
		}
		// pass #1
		{
			osg::ref_ptr<osg::StateSet> ss = new osg::StateSet;
			osg::Stencil *stencil = new osg::Stencil;
			stencil->setFunction(osg::Stencil::NOTEQUAL, 0x0, ~0);
			stencil->setOperation(osg::Stencil::ZERO, osg::Stencil::ZERO,osg::Stencil::ZERO);
			ss->setAttributeAndModes(stencil, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
			ss->setMode(GL_CLIP_PLANE0, osg::StateAttribute::OFF);
			osg::Depth *depth = new osg::Depth();
			depth->setWriteMask(true);
			ss->setAttributeAndModes( depth, osg::StateAttribute::ON);
			ss->setMode(GL_CULL_FACE,osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
			ss->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL));
			ss->setMode(GL_CLIP_PLANE0, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
			addPass(ss.get());
		}
	}

	virtual osg::Node *getOverrideChild(int pass){
		switch(pass) {
		case 1: // Second pass (pass #1) draws the cap plane
			return _capPlane;
			break;
		default:
			return NULL;
			break;
		}
	}
};

class CappingEffect : public osgFX::Effect {
public:
	CappingEffect() : osgFX::Effect() {}
	CappingEffect( const CappingEffect& copy, const osg::CopyOp op=osg::CopyOp::SHALLOW_COPY )
		: osgFX::Effect(copy, op) {}

	META_Effect( osgFX, CappingEffect, "CappingEffect", "", "" );
protected:
	virtual bool define_techniques(){
		CappingTechnique* technique = new CappingTechnique();
		addTechnique(technique);
		return true;
	}
};

