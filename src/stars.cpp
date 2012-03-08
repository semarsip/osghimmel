﻿
// Copyright (c) 2011-2012, Daniel Müller <dm@g4t3.de>
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

#include "stars.h"

#include "mathmacros.h"
#include "sideraltime.h"


const long double star_tempratureFromBV(const long double BtoV)
{
    // NOTE: This is just an estimation!

    // (Night Rendering - Jensen, Premovze, Shirley, Thompson, Ferwerda - 2001)
    return 7000.0 / (BtoV + 0.56);

    // (The Colors of the Stars - Olson - 1998)
    //return 1000.0 + 5000.0 / (BtoV + 0.5);
}


// Planckian locus in CIE XYZ Approximation

const osg::Vec2f star_planckianLocusInCieXYZ(const long double t)
{
    // (http://en.wikipedia.org/wiki/Planckian_locus)

    float x(0.f), y(0.f);

    const float o = 1.0 / t;

         if( 1667 <= t && t <=  4000)
        x = +0.17991 + o * (+877.6956 + o * (- 234358.0 + o * - 266123900.0));
    else if( 4000 <  t && t <= 25000) 
        x = +0.24039 + o * (+222.6347 + o * (+2107037.9 + o * -3025846900.0));


         if( 1667 <= t && t <=  2222)
        y = -0.20219683 + t * (+2.18555832 + t * (-1.3481102 + t * -1.1063814));
    else if( 2222 <  t && t <=  4000) 
        y = -0.16748867 + t * (+2.09137015 + t * (-1.3741859 + t * -0.9549476));
    else if( 4000 <  t && t <= 25000) 
        y = -0.37001483 + t * (+3.75112997 + t * (-5.8733867 + t * +3.0817580));

    return osg::Vec2f(x, y);
}


const osg::Vec3f star_xyzTristimulus(const long double t)
{
    return star_xyzTristimulus(star_planckianLocusInCieXYZ(t));
}

const osg::Vec3f star_xyzTristimulus(const osg::Vec2f planckianLocus)
{
    // (http://en.wikipedia.org/wiki/CIE_1931_color_space)
    // and (The Colors of the Stars - Olson - 1998)

    const float Y = 1.0;
    
    const float X = Y / planckianLocus.y() * planckianLocus.x();
    const float Z = Y / planckianLocus.y() * (1.0 - planckianLocus.x() - planckianLocus.y());

    return osg::Vec3f(X, Y, Z);
}


const osg::Vec3f star_sRgbColor(const long double t)
{
    return star_sRgbColor(star_xyzTristimulus(t));
}

const osg::Vec3f star_sRgbColor(const osg::Vec2f planckianLocus)
{
    return star_sRgbColor(star_xyzTristimulus(planckianLocus));
}

const osg::Vec3f star_sRgbColor(const osg::Vec3f xyzTristimulus)
{
    // (The Colors of the Stars - Olson - 1998) - sRGB matrix - feel free to use other matrices here...

    const float r =  3.24  * xyzTristimulus.x() -1.537 * xyzTristimulus.y() -0.499 * xyzTristimulus.z();
    const float g = -0.969 * xyzTristimulus.x() +1.876 * xyzTristimulus.y() +0.042 * xyzTristimulus.z();
    const float b =  0.056 * xyzTristimulus.x() -0.204 * xyzTristimulus.y() +1.057 * xyzTristimulus.z();

    return osg::Vec3f(r, g, b);
}


const t_equd star_apparentPosition(
    const t_julianDay t
,   const long double α2000
,   const long double δ2000
,   const long double mpα2000
,   const long double mpδ2000)
{
    const t_julianDay T(jCenturiesSinceSE(t));

    // (AA.20.1)

    const long double m  = (_decimal(0, 0, 3.07496)) + (_decimal(0, 0, 0.00186)) * T * 100.0;
    const long double n  = _arcsecs(_decimal(0, 0, 1.33621)) - _arcsecs(_decimal(0, 0, 0.00057)) * T * 100.0;

    const long double Δα = m + n * sin(_rad(mpα2000)) * tan(_rad(mpδ2000));
    const long double Δβ = n * cos(_rad(mpα2000));

    t_equd equ;

    //equ.right_ascension = 




    return equ;
}


const t_hord star_horizontalPosition(
    const t_aTime &aTime
,   const long double latitude
,   const long double longitude
,   const long double α2000
,   const long double δ2000
,   const long double mpα2000
,   const long double mpδ2000)
{
    t_julianDay t(jd(aTime));
    t_julianDay s(siderealTime(aTime));

    t_equd equ = star_apparentPosition(t, α2000, δ2000, mpα2000, mpδ2000);

    return equ.toHorizontal(s, latitude, longitude);
}