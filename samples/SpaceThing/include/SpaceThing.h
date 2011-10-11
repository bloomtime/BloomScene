//
//  SpaceThing.h
//  Test
//
//  Created by Tom Carden on 10/4/11.
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#pragma once
#include "BloomNode.h"
#include "cinder/Vector.h"

typedef std::shared_ptr<class SpaceThing> SpaceThingRef;

class SpaceThing : public BloomNode
{
public:

    static SpaceThingRef create(float planetRadius, float orbitRadius, float orbitAngle, float orbitVelocity)
    {
        return SpaceThingRef(new SpaceThing(planetRadius, orbitRadius, orbitAngle, orbitVelocity));
    }
    
    ~SpaceThing() {}
    
    void update();
    void draw();
    
private:

    SpaceThing(float planetRadius, float orbitRadius, float orbitAngle, float orbitVelocity): 
        mPlanetRadius(planetRadius),
        mOrbitRadius(orbitRadius),
        mOrbitAngle(orbitAngle),
        mOrbitVelocity(orbitVelocity)
    {}

    float mPlanetRadius;
    float mOrbitRadius;
    float mOrbitAngle;
    float mOrbitVelocity;
    
};