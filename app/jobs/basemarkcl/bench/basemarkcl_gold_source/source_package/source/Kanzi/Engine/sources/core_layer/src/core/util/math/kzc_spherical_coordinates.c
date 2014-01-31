/**
* \file
* Spherical coordinate system utilities.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzc_spherical_coordinates.h"


struct KzcVector3 kzcSphericalCoordinatesToCartesian(kzFloat radius, kzFloat phi, kzFloat theta)
{
    kzFloat sinPhi = kzsSinf(phi);
    kzFloat x = radius * sinPhi * kzsCosf(theta);
    kzFloat y = radius * sinPhi * kzsSinf(theta);
    kzFloat z = radius * kzsCosf(phi);
    return kzcVector3(x, y, z);
}

void kzcSphericalCoordinatesFromCartesian(struct KzcVector3 cartesian, kzFloat* out_radius, kzFloat* out_phi, kzFloat* out_theta)
{
    kzFloat radius = kzcVector3GetLength(&cartesian);
    kzFloat phi = kzsAcosf(cartesian.data[2] / radius);
    kzFloat theta = kzsAtan2f(cartesian.data[1], cartesian.data[0]);
    
    *out_radius = radius;
    *out_phi = phi;
    *out_theta = theta;
}
