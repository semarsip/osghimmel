
// Copyright (c) 2011-2012, Daniel M�ller <dm@g4t3.de>
// Computer Graphics Systems Group at the Hasso-Plattner-Institute, Germany
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
//   * Redistributions of source code must retain the above copyright notice, 
//     this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright 
//     notice, this list of conditions and the following disclaimer in the 
//     documentation and/or other materials provided with the distribution.
//   * Neither the name of the Computer Graphics Systems Group at the 
//     Hasso-Plattner-Institute (HPI), Germany nor the names of its 
//     contributors may be used to endorse or promote products derived from 
//     this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGE.

#include "astronomy.h"

#include "earth.h"
#include "sun.h"
#include "moon.h"
#include "stars.h"
#include "sideraltime.h"


Astronomy::Astronomy()
{
}


const float Astronomy::angularSunRadius(const t_julianDay t) const
{
    return earth_apparentAngularSunDiameter(t) * 0.5;
}


const float Astronomy::angularMoonRadius(const t_julianDay t) const
{
    return earth_apparentAngularMoonDiameter(t) * 0.5;
}


const osg::Vec3 Astronomy::moonPosition(
    const t_aTime &aTime
,   const float latitude
,   const float longitude) const
{
    t_hord moon = moon_horizontalPosition(aTime, latitude, longitude);

    osg::Vec3 moonv  = moon.toEuclidean();
    moonv.normalize();

    return moonv;
}


const osg::Vec3 Astronomy::sunPosition(
    const t_aTime &aTime
,   const float latitude
,   const float longitude) const
{
    t_hord sun = sun_horizontalPosition(aTime, latitude, longitude);

    osg::Vec3 sunv  = sun.toEuclidean();
    sunv.normalize();

    return sunv;
}


const osg::Matrix Astronomy::equToLocalHorizonMatrix() const
{
    const t_aTime aTime(getATime());

    const float s = siderealTime(aTime);

    const float la = getLatitude();
    const float lo = getLongitude();

    return osg::Matrix::scale                  (-1, 1, 1)
        * osg::Matrix::rotate( _rad(la) - _PI_2, 1, 0, 0)
        * osg::Matrix::rotate(-_rad(s + lo)    , 0, 0, 1);
}