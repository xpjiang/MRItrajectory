/***************************************************************************

 Copyright (c) 2014 The Board of Trustees of the Leland Stanford Junior University.
 All rights reserved.
 Contact: Okai Addy <noaddy@alumni.stanford.edu>

 This source code is under a BSD 3-Clause License.
 See LICENSE for more information.

To distribute this file, substitute the full license for the above reference.

**************************************************************************/
#ifndef RADIAL_H
#define RADIAL_H

#include "angles.h"

struct Trajectory;

struct Trajectory* generateRadial2D(float fovx, float fovy, enum AngleShape thetaShape, float resx, float resy, enum AngleShape kShape, int fullProjection, float pct, float gmax, float maxSlewRate, float Ts);

struct Trajectory* generateRadial3D(float fovx, float fovy, float fieldOfViewZ, enum AngleShape thetaShape, enum AngleShape phiShape, float resx, float resy, float spatialResolutionZ, int fullProjection, float pct, float gmax, float maxSlewRate, float Ts);


#endif // RADIAL_H
