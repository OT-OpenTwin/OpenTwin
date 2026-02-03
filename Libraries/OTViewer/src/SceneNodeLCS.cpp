// @otlicense
// File: SceneNodeLCS.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#include "OTViewer/SceneNodeLCS.h"

#include <osg/Matrix>
#include <osg/Vec3d>
#include <cmath>

void SceneNodeLCS::updateTransformationMatrix()
{
    transformation = makeOrthonormalFrame(osg::Vec3d(origin[0], origin[1], origin[2]), osg::Vec3d(xAxis[0], xAxis[1], xAxis[2]), osg::Vec3d(zAxis[0], zAxis[1], zAxis[2]));
}

// Build an orthonormal coordinate frame from a non-orthogonal
// X-axis and Z-axis using Gram-Schmidt orthogonalization.
// The resulting matrix represents a pure rotation + translation
// (no shear, no scaling).
osg::Matrix SceneNodeLCS::makeOrthonormalFrame(const osg::Vec3d& origin, osg::Vec3d xAxis, osg::Vec3d zAxis)
{
    // ------------------------------------------------------------------
    // Step 1: Normalize the X axis
    // ------------------------------------------------------------------
    if (xAxis.length2() < 1e-20) {
        // Degenerate input: X axis is too small
        return osg::Matrix::identity();
    }
    xAxis.normalize();

    // ------------------------------------------------------------------
    // Step 2: Make Z orthogonal to X (Gram-Schmidt)
    // z_orth = z - proj_x(z)
    // ------------------------------------------------------------------
    osg::Vec3d zTemp = zAxis - xAxis * (zAxis * xAxis); // (z*x) * x

    if (zTemp.length2() < 1e-20) {
        // Z was nearly parallel to X, choose a fallback direction
        osg::Vec3d tmp =
            (std::abs(xAxis.z()) < 0.9) ? osg::Vec3d(0, 0, 1)
            : osg::Vec3d(0, 1, 0);

        zTemp = tmp - xAxis * (tmp * xAxis);
    }
    zTemp.normalize();

    // ------------------------------------------------------------------
    // Step 3: Compute Y axis using a right-handed coordinate system
    // Y = Z x X
    // ------------------------------------------------------------------
    osg::Vec3d yAxis = zTemp ^ xAxis;
    yAxis.normalize();

    // ------------------------------------------------------------------
    // Step 4 (optional but recommended):
    // Recompute Z to improve numerical stability
    // Z = X x Y
    // ------------------------------------------------------------------
    osg::Vec3d zAxisOrtho = xAxis ^ yAxis;
    zAxisOrtho.normalize();

    // ------------------------------------------------------------------
    // Step 5: Build the transformation matrix
    // OpenSceneGraph expects basis vectors as columns (column-major)
    // ------------------------------------------------------------------
    return osg::Matrix(
        xAxis.x(), xAxis.y(), xAxis.z(), 0.0,
        yAxis.x(), yAxis.y(), yAxis.z(), 0.0,
        zAxisOrtho.x(), zAxisOrtho.y(), zAxisOrtho.z(), 0.0,
        origin.x(), origin.y(), origin.z(), 1.0
    );
}
