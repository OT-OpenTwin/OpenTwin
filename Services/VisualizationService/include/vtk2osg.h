/*
 * Copyright (C) 2002-2003 Michael Gronager, UNI-C
 *
 * Distributed under the terms of the GNU Library General Public License (LGPL)
 * as published by the Free Software Foundation.
 */
 

#ifndef VTK_ACTOR_TO_OSG_H_
#define VTK_ACTOR_TO_OSG_H_

#include <vtkActor.h>
#include <vtkCellArray.h>

#include <osg/Geometry>
#include <osg/Node>
#include <osg/PrimitiveSet>

/**
 * @brief Translates vtkActor to osg::Node.
 */
osg::Node *VTKActorToOSG(vtkActor *actor);

/**
 * @brief Process primitives.
 */
osg::Geometry *processPrimitive(vtkActor *actor, vtkCellArray *prim_array, osg::PrimitiveSet::Mode prim_type);

#endif  // VTK_ACTOR_TO_OSG_H_
