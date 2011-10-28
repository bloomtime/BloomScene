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
    
    Vec3f vertices[16];

    // begin at 0,0
    vertices[0].x = 0.0f;
    vertices[0].y = 0.0f;
    vertices[0].z = 0.0f;

    for (int i = 1; i < 16; i++) {
        float a = M_PI * 2.0f * (float)(i-1) / 14.0f;
        vertices[i].x = std::cos(a) * mPlanetRadius;
        vertices[i].y = std::sin(a) * mPlanetRadius;
        vertices[i].z = 0.0f;
    }

    GLfloat colors[64];
    for (int i = 0; i < 64; i++) {
        colors[i] = 1.0f;
    }
    
    mProg.uniform("Modelview", getConcatenatedTransform());

    GLuint vertexAttr = mProg.getAttribLocation("Position"); 
    GLuint colorAttr = mProg.getAttribLocation("SourceColor");
    
    glEnableVertexAttribArray(vertexAttr);
    glEnableVertexAttribArray(colorAttr);

    glVertexAttribPointer(vertexAttr, 3, GL_FLOAT, 0, 0, vertices);    
    glVertexAttribPointer(colorAttr, 4, GL_FLOAT, 0, 0, colors);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 16);    

    glDisableVertexAttribArray(vertexAttr);
    glDisableVertexAttribArray(colorAttr);

    // and then any children will be draw after this
}
