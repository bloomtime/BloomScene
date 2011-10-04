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

class SpaceThing : public BloomNode
{
public:
    
    SpaceThing(float planetRadius, float orbitRadius, float orbitAngle, float orbitVelocity): 
        mPlanetRadius(planetRadius),
        mOrbitRadius(orbitRadius),
        mOrbitAngle(orbitAngle),
        mOrbitVelocity(orbitVelocity)
    {}
    
    ~SpaceThing() {}
    
    void update();
    void draw();
    
private:
    
    float mPlanetRadius;
    float mOrbitRadius;
    float mOrbitAngle;
    float mOrbitVelocity;
    
};