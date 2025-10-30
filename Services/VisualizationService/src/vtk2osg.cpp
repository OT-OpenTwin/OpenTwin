// @otlicense-ignore
/*
 * Copyright (C) 2002-2003 Michael Gronager, UNI-C
 *
 * Distributed under the terms of the GNU Library General Public License (LGPL)
 * as published by the Free Software Foundation.
 */

#include "stdafx.h"
 
#include "vtk2osg.h"

#include <vtkCellData.h>
#include <vtkDataSet.h>
#include <vtkMapper.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkUnsignedCharArray.h>

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/LineWidth>
#include <osg/Vec3>
#include <osg/PolygonMode>

osg::Node *VTKActorToOSG(vtkActor *actor) {
  actor->GetMapper()->Update();

  if (strcmp(actor->GetMapper()->GetInput()->GetClassName(), "vtkPolyData")) {
    std::cout << "ERROR! Actor must use a vtkPolyDataMapper." << std::endl;
    return NULL;
  }

  osg::ref_ptr<osg::Geode> geode = new osg::Geode();
  vtkPolyData *poly_data = reinterpret_cast<vtkPolyData *>(actor->GetMapper()->GetInput());

  osg::ref_ptr<osg::Geometry> points, lines, polys, strips;
  points = processPrimitive(actor, poly_data->GetVerts(), osg::PrimitiveSet::POINTS);
  lines = processPrimitive(actor, poly_data->GetLines(), osg::PrimitiveSet::LINE_STRIP);
  polys = processPrimitive(actor, poly_data->GetPolys(), osg::PrimitiveSet::POLYGON);
  strips = processPrimitive(actor, poly_data->GetStrips(), osg::PrimitiveSet::TRIANGLE_STRIP);

  if (points.valid()) {
    geode->addDrawable(points.get());
  }
  if (lines.valid()) {
    geode->addDrawable(lines.get());
  }
  if (polys.valid()) {
    geode->addDrawable(polys.get());
  }
  if (strips.valid()) {
    geode->addDrawable(strips.get());
  }

  return geode.release();
}

osg::Geometry *processPrimitive(vtkActor *actor, vtkCellArray *prim_array, osg::PrimitiveSet::Mode prim_type) {
  vtkPolyData *poly_data = reinterpret_cast<vtkPolyData *>(actor->GetMapper()->GetInput());

  if (prim_array->GetNumberOfCells() == 0) {
    return NULL;
  }

  osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();
  osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
  osg::ref_ptr<osg::Vec3Array> norms = new osg::Vec3Array();
  osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
  osg::ref_ptr<osg::Vec2Array> tcoords = new osg::Vec2Array();

  // Check to see if there are normals
  int normal_per_vertex = 0;
  int normal_per_cell = 0;
  vtkDataArray *normals = poly_data->GetPointData()->GetNormals();

  if (actor->GetProperty()->GetInterpolation() == VTK_FLAT) {
    normals = NULL;
  }

  if (normals != NULL) {
    normal_per_vertex = 1;
  } else {
    normals = poly_data->GetCellData()->GetNormals();
    if (normals != NULL) {
      normal_per_cell = 1;
    }
  }

  // Check to see if there is color information
  int color_per_vertex = 0;
  int color_per_cell = 0;
  vtkUnsignedCharArray *color_array = actor->GetMapper()->MapScalars(1.0);

  if (actor->GetMapper()->GetScalarVisibility() && color_array != NULL) {
    int scalar_mode = actor->GetMapper()->GetScalarMode();

    if (scalar_mode == VTK_SCALAR_MODE_USE_CELL_DATA || !poly_data->GetPointData()->GetScalars()) {
      color_per_cell = 1;
    } else {
      color_per_vertex = 1;
    }
  }

  // Check to see if there are texture coordinates
  vtkDataArray *tex_coords = poly_data->GetPointData()->GetTCoords();

  // Copy data from vtk prim array to osg Geometry
  int prim = 0;
  int vert = 0;
  int tot_pts = 0;
  bool transparent_flag = false;
  vtkIdType npts;
  const vtkIdType *pts;

  // Go through cells (primitives)
  for (prim_array->InitTraversal(); prim_array->GetNextCell(npts, pts); prim++) {
    geom->addPrimitiveSet(new osg::DrawArrays(prim_type, tot_pts, npts));
    tot_pts += npts;

    if (color_per_cell) {
      unsigned char *aColor = color_array->GetPointer(4 * prim);
      colors->push_back(osg::Vec4(aColor[0] / 255.0f, aColor[1] / 255.0f, aColor[2] / 255.0f, aColor[3] / 255.0f));

      if (aColor[3] / 255.0f < 1) {
        transparent_flag = true;
      }
    }

    if (normal_per_cell) {
      double *aNormal = normals->GetTuple(prim);
      norms->push_back(osg::Vec3(aNormal[0], aNormal[1], aNormal[2]));
    }

    // Go through points in cell (verts)
    for (int i = 0; i < npts; i++) {
      double *aVertex = poly_data->GetPoint(pts[i]);
      vertices->push_back(osg::Vec3(aVertex[0], aVertex[1], aVertex[2]));

      if (normal_per_vertex) {
        double *aNormal = normals->GetTuple(pts[i]);
        norms->push_back(osg::Vec3(aNormal[0], aNormal[1], aNormal[2]));
      }

      if (color_per_vertex) {
        unsigned char *aColor = color_array->GetPointer(4 * pts[i]);
        colors->push_back(osg::Vec4(aColor[0] / 255.0f, aColor[1] / 255.0f, aColor[2] / 255.0f, aColor[3] / 255.0f));

        if (aColor[3] / 255.0f < 1) {
          transparent_flag = true;
        }
      }

      if (tex_coords != NULL) {
        double *aTCoord = tex_coords->GetTuple(pts[i]);
        tcoords->push_back(osg::Vec2(aTCoord[0], aTCoord[1]));
      }

      vert++;
    }
  }

  // Add attribute arrays to gset
  geom->setVertexArray(vertices.get());
  geom->setColorArray(colors.get());

  if (normals) {
    geom->setNormalArray(norms.get());
  }
  if (normal_per_vertex) {
    geom->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
  }
  if (normal_per_cell) {
    geom->setNormalBinding(osg::Geometry::BIND_PER_PRIMITIVE_SET);
  }

  if (color_per_vertex) {
    geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
  } else if (color_per_cell) {
    geom->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE_SET);
  } else {
    double *actor_color = actor->GetProperty()->GetColor();
    double opacity = actor->GetProperty()->GetOpacity();

    colors->push_back(osg::Vec4(actor_color[0], actor_color[1], actor_color[2], opacity));
    geom->setColorBinding(osg::Geometry::BIND_OVERALL);
  }

  if (tex_coords != NULL) {
    geom->setTexCoordArray(0, tcoords.get());
  }

  osg::ref_ptr<osg::StateSet> state = geom->getOrCreateStateSet();

  if (actor->GetProperty()->GetOpacity() < 1.0 || transparent_flag) {
    state->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
    state->setMode(GL_BLEND, osg::StateAttribute::ON);
    state->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);
  }

  if (actor->GetProperty()->GetRepresentation() == VTK_WIREFRAME) {
    osg::ref_ptr<osg::LineWidth> line_width = new osg::LineWidth;
    line_width->setWidth(actor->GetProperty()->GetLineWidth());
    state->setAttributeAndModes(line_width.get(), osg::StateAttribute::ON);
	state->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE));
	state->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
  }

  if (!actor->GetProperty()->GetBackfaceCulling()) {
    state->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);
  }

  if (normals != NULL && actor->GetProperty()->GetRepresentation() != VTK_WIREFRAME) {
    state->setMode(GL_LIGHTING, osg::StateAttribute::ON);
  } else {
    state->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  }

  if (prim_type == osg::PrimitiveSet::LINE_STRIP) {
    state->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  }

  return geom.release();
}
