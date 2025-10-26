#include "stdafx.h"
#include "HandleWheel.h"
#include "ViewerSettings.h"
#include "Viewer.h"

#include <osg/Switch>
#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>
#include <osg/PolygonMode>
#include <osg/LightModel>
#include <osg/Material>
#include <osg/ShadeModel>

#include <cassert>

HandleWheel::HandleWheel(osg::Vec3d point, osg::Vec3d normal, double radius, double tubeRadius, double color[4])
{
	// Create the osg node
	osgNode = new osg::MatrixTransform;

	wheelPosition = point;
	wheelNormal = normal;
	wheelRadius = radius;

	updateTransform();

	numberSegments = 72;

	osgSwitchNode = new osg::Switch;
	osgNode->addChild(osgSwitchNode);

	wheelNode = new osg::Geode;
	createWheel(wheelNode, color, wheelRadius, tubeRadius, numberSegments);

	wheelNodeSelected = new osg::Geode;
	createWheel(wheelNodeSelected, color, wheelRadius, 1.3 * tubeRadius, numberSegments);

	std::list<osg::Vec3d> pointList;
	for (int i = 0; i <= numberSegments; i++)
	{
		double x = wheelRadius * cos(i * 2 * 3.14159265358979323846 / numberSegments);
		double y = wheelRadius * sin(i * 2 * 3.14159265358979323846 / numberSegments);

		pointList.push_back(osg::Vec3d(x, y, 0.0));
	}
	createSelectLine(osgSwitchNode, pointList, false);

	segmentLength = 2.0 * wheelRadius * sin(2 * 3.14159265358979323846 / numberSegments * 0.5);

	osgSwitchNode->addChild(wheelNode);
	osgSwitchNode->addChild(wheelNodeSelected);

	osgSwitchNode->setChildValue(wheelNodeSelected, false);

	referenceAngleDeg = 0.0;
	wheelRotationAngleDeg = 0.0;
	rotationAngleDeg = 0.0;
}

HandleWheel::~HandleWheel()
{

}

osg::Node *HandleWheel::getNode(void)
{
	return osgNode;
}

void HandleWheel::createWheel(osg::Geode * _geode, double color[4], double wheelRadius, double tubeRadius, int numberSegments) 
{
	// Create the torus geometry

	float start_sweep = 0.0;
	float end_sweep = osg::DegreesToRadians(360.0);

	int torus_sweeps = numberSegments;
	int circle_sweeps = numberSegments / 2;

	float dsweep = (end_sweep - start_sweep)/(float)torus_sweeps;
	float dphi = osg::DegreesToRadians(360.0) / (float)circle_sweeps;

	for(int j=0; j<circle_sweeps; j++)
	{
		osg::Vec3Array* vertices = new osg::Vec3Array;
		osg::Vec3Array* normals = new osg::Vec3Array;

		float phi = dphi*(float)j;
		float cosPhi = cosf(phi);
		float sinPhi = sinf(phi);
		float next_cosPhi = cosf(phi+dphi);
		float next_sinPhi = sinf(phi+dphi);
		float overnext_cosPhi = cosf(phi+2.0f*dphi);
		float overnext_sinPhi = sinf(phi+2.0f*dphi);

		float z = tubeRadius*sinPhi;
		float yPrime = wheelRadius + tubeRadius*cosPhi;

		float next_z = tubeRadius*next_sinPhi;
		float overnext_z = tubeRadius*overnext_sinPhi;
		float next_yPrime = wheelRadius + tubeRadius*next_cosPhi;
		float overnext_yPrime = wheelRadius + tubeRadius*overnext_cosPhi;

		float old_x = yPrime*cosf(-dsweep);
		float old_y = yPrime*sinf(-dsweep);
		float old_z = z;

		for(int i=0; i<torus_sweeps; ++i)
		{
			float sweep = start_sweep + dsweep*i;
			float cosSweep = cosf(sweep);
			float sinSweep = sinf(sweep);

			float x = yPrime*cosSweep;
			float y = yPrime*sinSweep;

			float next_x = next_yPrime*cosSweep;
			float next_y = next_yPrime*sinSweep;
			float overnext_x = overnext_yPrime*cosSweep;
			float overnext_y = overnext_yPrime*sinSweep;

			vertices->push_back( osg::Vec3(next_x,next_y,next_z) );
			vertices->push_back( osg::Vec3(x,y,z) );

			// calculate normals
			osg::Vec3 lateral(next_x-x,next_y-y,next_z-z);
			osg::Vec3 nextlateral(overnext_x-next_x,overnext_y-next_y,overnext_z-next_z);
			osg::Vec3 longitudinal(x-old_x,y-old_y,z-old_z);
			osg::Vec3 normal = longitudinal ^ lateral;  // cross product
			osg::Vec3 nextnormal = longitudinal ^ nextlateral;  // cross product
			normal.normalize();
			nextnormal.normalize();

			normals->push_back( nextnormal );
			normals->push_back( normal );

			old_x = x; old_y = y; old_z = z;
		} // end torus loop

		 // the last point
		float last_sweep = start_sweep + end_sweep;
		float cosLastSweep = cosf(last_sweep);
		float sinLastSweep = sinf(last_sweep);

		float x = yPrime*cosLastSweep;
		float y = yPrime*sinLastSweep;

		float next_x = next_yPrime*cosLastSweep;
		float next_y = next_yPrime*sinLastSweep;
		float overnext_x = overnext_yPrime*cosLastSweep;
		float overnext_y = overnext_yPrime*sinLastSweep;

		vertices->push_back( osg::Vec3(next_x,next_y,next_z) );
		vertices->push_back( osg::Vec3(x,y,z) );

		osg::Vec3 lateral(next_x-x,next_y-y,next_z-z);
		osg::Vec3 nextlateral(overnext_x-next_x,overnext_y-next_y,overnext_z-next_z);
		osg::Vec3 longitudinal(x-old_x,y-old_y,z-old_z);
		osg::Vec3 norm = longitudinal ^ lateral;
		osg::Vec3 nextnorm = longitudinal ^ nextlateral;
		norm.normalize();
		nextnorm.normalize();

		normals->push_back( nextnorm );
		normals->push_back( norm );

		osg::ShadeModel* shademodel = new osg::ShadeModel;
		shademodel->setMode( osg::ShadeModel::SMOOTH );

		osg::StateSet* stateset = new osg::StateSet;
		stateset->setAttribute( shademodel );

		osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;

		if (!ViewerSettings::instance()->useDisplayLists)
		{
			geometry->setUseDisplayList(false);
			geometry->setUseVertexBufferObjects(ViewerSettings::instance()->useVertexBufferObjects);
		}

		geometry->setStateSet( stateset );
		geometry->setVertexArray( vertices );

		osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(1);
		colors->at(0).set(color[0], color[1], color[2], color[3]);
		geometry->setColorArray(colors);
		geometry->setColorBinding(osg::Geometry::BIND_OVERALL);

		geometry->setNormalArray( normals );
		geometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

		geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP, 0, vertices->size()) );
		_geode->addDrawable( geometry );
	}  // end cirle loop

	// Apply the general settings

	osg::StateSet * ssGeode = _geode->getOrCreateStateSet();

	ssGeode->setMode(GL_LIGHTING, osg::StateAttribute::ON);
	ssGeode->setMode(GL_BLEND, osg::StateAttribute::ON);
	ssGeode->setMode(GL_POINT_SMOOTH, osg::StateAttribute::ON);

	osg::ref_ptr<osg::Material> material = new osg::Material;

	material->setAmbient(osg::Material::FRONT_AND_BACK,  osg::Vec4(0.5f * color[0], 0.5f *  color[1], 0.5f * color[2], 1.0f));
	material->setDiffuse(osg::Material::FRONT_AND_BACK,  osg::Vec4(0.8f *  color[0], 0.8f * color[1], 0.8f * color[2], 1.0f));
	material->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0f    , 0.0f    , 0.0f    , 1.0f));

	material->setColorMode(osg::Material::OFF);

	material->setAlpha(osg::Material::FRONT_AND_BACK, color[3]);

	material->setShininess(osg::Material::FRONT_AND_BACK, 0);

	ssGeode->setAttribute(material);
}

void HandleWheel::updateTransform(void)
{
	osg::Matrix matrixRotate;
	matrixRotate.makeRotate(osg::Vec3d(0.0, 0.0, 1.0), wheelNormal);

	osg::Matrix matrixTranslate;
	matrixTranslate.makeTranslate(wheelPosition);

	osgNode->setMatrix(matrixRotate * matrixTranslate);
}

void HandleWheel::setPosition(osg::Vec3d pos)
{
	wheelPosition = pos;
	updateTransform();
}

void HandleWheel::mouseOver(bool flag)
{
	osgSwitchNode->setChildValue(wheelNode, !flag);
	osgSwitchNode->setChildValue(wheelNodeSelected, flag);
}

void HandleWheel::setInteractionReference(int intersectionIndex, double intersectionRatio)
{
	// Calculate the new reference offset
	wheelRotationAngleDeg = rotationAngleDeg;
	referenceAngleDeg = calculateAngle(intersectionIndex, intersectionRatio);
}

void HandleWheel::setInteraction(Viewer *viewer, int intersectionIndex, double intersectionRatio)
{
	// Calculate the new angle
	rotationAngleDeg = calculateAngle(intersectionIndex, intersectionRatio) - referenceAngleDeg + wheelRotationAngleDeg;

	rotationAngleDeg = viewer->snapAngle(rotationAngleDeg);

	reportInteraction();
}

double HandleWheel::calculateAngle(int intersectionIndex, double intersectionRatio)
{
	double totalLength = numberSegments * segmentLength;
	double currentLength = intersectionIndex * segmentLength + intersectionRatio * segmentLength;

	double angle = currentLength / totalLength * 360.0;

	return angle;
}
