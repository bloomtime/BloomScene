//
//  SpaceThing.cpp
//  Test
//
//  Created by Tom Carden on 10/4/11.
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#include "SpaceThing.h"
#include "cinder/gl/gl.h"

using namespace ci;

void SpaceThing::update()
{
    mOrbitAngle += mOrbitVelocity;
    
    const float TWO_PI = 2.0f * M_PI;
    if (mOrbitAngle < 0) {
        mOrbitAngle += TWO_PI;
    }
    else if (mOrbitAngle > TWO_PI) {
        mOrbitAngle -= TWO_PI;
    }
    
    Matrix44f mat = getTransform();
    mat.setToIdentity();
    mat.rotate( Vec3f::zAxis(), mOrbitAngle );
    mat.translate( Vec3f(mOrbitRadius, 0.0f, 0.0f) );
    setTransform(mat);
}

void SpaceThing::draw()
{
    // Matrix is already applied so we can draw at origin
    gl::color( Color::white() );
    gl::drawSolidCircle( Vec2f::zero(), mPlanetRadius );
    
    // and then any children will be draw after this
}
