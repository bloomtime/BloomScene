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
//    gl::color( Color::white() );
//    gl::drawSolidCircle( Vec2f::zero(), mPlanetRadius );
    
    static GLfloat squareVertices[12] = {
        -mPlanetRadius, -mPlanetRadius, 0.0,
        mPlanetRadius, -mPlanetRadius, 0.0,
        -mPlanetRadius,  mPlanetRadius, 0.0,
        mPlanetRadius,  mPlanetRadius, 0.0
    };

    static GLfloat squareColors[16] = {
        1.0, 1.0, 1.0, 1.0,
        1.0, 1.0, 1.0, 1.0,
        1.0, 1.0, 1.0, 1.0,
        1.0, 1.0, 1.0, 1.0
    };

    mProg.uniform("Modelview", getConcatenatedTransform());
    
    mProg.bind();
    
    GLuint vertexAttr = mProg.getAttribLocation("Position"); 
    GLuint colorAttr = mProg.getAttribLocation("SourceColor");
    
    glEnableVertexAttribArray(vertexAttr);
    glEnableVertexAttribArray(colorAttr);
    
    glVertexAttribPointer(vertexAttr, 3, GL_FLOAT, 0, 0, squareVertices);    
    glVertexAttribPointer(colorAttr, 4, GL_FLOAT, 0, 0, squareColors);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);    
    
    glDisableVertexAttribArray(vertexAttr);
    glDisableVertexAttribArray(colorAttr);

    mProg.unbind();
    
    // and then any children will be draw after this
}
