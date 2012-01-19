
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

#pragma once
#ifndef __PROCEDURALHIMMEL_H__
#define __PROCEDURALHIMMEL_H__

#include "abstracthimmel.h"


class AbstractAstronomy;
class AtmosphereGeode;
class MoonGeode;
class StarsGeode;
class GalaxyGeode;


class ProceduralHimmel : public AbstractHimmel
{
public:

    ProceduralHimmel();
    virtual ~ProceduralHimmel();


    const float setLatitude(const float latitude);
    const float getLatitude() const;

    const float setLongitude(const float longitude);
    const float getLongitude() const;


    inline AtmosphereGeode *atmosphere() const 
    {
        return m_atmosphere;
    }

    inline MoonGeode *moon() const
    {
        return m_moon;
    }

    inline StarsGeode *stars() const
    {
        return m_stars;
    }

    inline GalaxyGeode *galaxy() const
    {
        return m_galaxy;
    }


    inline AbstractAstronomy *astro() const
    {
        return m_astronomy;
    }

protected:

    virtual void update();

protected:

    AbstractAstronomy *m_astronomy;

    osg::ref_ptr<AtmosphereGeode>   m_atmosphere;
    osg::ref_ptr<MoonGeode>         m_moon;
    osg::ref_ptr<StarsGeode>        m_stars;
    osg::ref_ptr<GalaxyGeode>       m_galaxy;
};

#endif // __PROCEDURALHIMMEL_H__