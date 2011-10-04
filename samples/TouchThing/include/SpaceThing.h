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
    
    ~SpaceThing();
    
    void update();
    void draw();
    
    // touchEnded will only be called for touch IDs that returned true in touchBegan
    // so we need to implement both methods (there's also touchMoved which works the same way)
    bool touchBegan( ci::app::TouchEvent::Touch touch );
    bool touchEnded( ci::app::TouchEvent::Touch touch );
    
    // we'll implement hitTest as a helper for touchBegan/touchEnded
    bool hitTest( const ci::Vec2f &screenPos );
    
private:
    
    float mPlanetRadius;
    float mOrbitRadius;
    float mOrbitAngle;
    float mOrbitVelocity;
};