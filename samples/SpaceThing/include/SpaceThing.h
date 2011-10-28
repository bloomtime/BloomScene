//
//  SpaceThing.h
//  Test
//
//  Created by Tom Carden on 10/4/11.
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#pragma once
#include "BloomNode.h"
#include <string>
#include "cinder/Vector.h"
#include "cinder/gl/GlslProg.h"

typedef std::shared_ptr<class SpaceThing> SpaceThingRef;

class SpaceThing : public BloomNode
{
public:

    static SpaceThingRef create(ci::gl::GlslProg prog, float planetRadius, float orbitRadius, float orbitAngle, float orbitVelocity)
    {
        return SpaceThingRef(new SpaceThing(prog, planetRadius, orbitRadius, orbitAngle, orbitVelocity));
    }
    
    ~SpaceThing() {}
    
    void update();
    void draw();
    
private:

    SpaceThing(ci::gl::GlslProg prog, float planetRadius, float orbitRadius, float orbitAngle, float orbitVelocity): 
        mProg(prog),
        mPlanetRadius(planetRadius),
        mOrbitRadius(orbitRadius),
        mOrbitAngle(orbitAngle),
        mOrbitVelocity(orbitVelocity)
    {}

    float mPlanetRadius;
    float mOrbitRadius;
    float mOrbitAngle;
    float mOrbitVelocity;
    
    ci::gl::GlslProg mProg;
};
