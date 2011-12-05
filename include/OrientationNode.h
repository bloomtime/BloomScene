//
//  OrientationNode.h
//  Kepler
//
//  Created by Tom Carden on 7/23/11.
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#pragma once

#include <map>
#include <string>
#include "cinder/Function.h"
#include "BloomNode.h"

// forward declare
typedef std::shared_ptr<class OrientationNode> OrientationNodeRef;

// declare this function outside of the class so that Objective-C blocks can be used
void setupNotifications( OrientationNode *node );

class OrientationNode : public BloomNode {
    
public:

    // our own shorter constants for Apple's long ones :)
    enum Orientation { 
        UNKNOWN,
        PORTRAIT,             // home button bottom
        UPSIDE_DOWN_PORTRAIT, // home button top
        LANDSCAPE_LEFT,       // home button right
        LANDSCAPE_RIGHT,      // home button left
        FACE_UP,
        FACE_DOWN
    };
    
    static OrientationNodeRef create();
    virtual ~OrientationNode();
        
    Orientation getInterfaceOrientation() { return mInterfaceOrientation; }
    void setInterfaceOrientation( const Orientation &orientation, bool animate = true );

    // TODO: get/setDeviceOrientation
    
    void enableAnimation( bool enable = true ) { mEnableAnimation = enable; }
    bool isAnimationEnabled() { return mEnableAnimation; }
    
    // the current animation angle (can be animating)
    // useful for syncing 3D world to 2D UI animation
    float getInterfaceAngle() { return mInterfaceAngle; }

    // the angle applied when the device is at this orientation
    float getOrientationAngle( const Orientation &orientation );
    
    // useful for debugging
    std::string getOrientationDescription( const Orientation &orientation );

    // animate mRoot interfaceSize and this node's transform matrix
    virtual void update( float elapsedSeconds );

    // TODO: make private and make setupNotifications a friend function?
    void orientationChanged( const Orientation &orientation );

    bool addedToScene();
    
    static Orientation convertOrientation( int uiDeviceOrienation );
    
protected:
    
    OrientationNode();
            
    bool mEnableAnimation;
    bool mCurrentlyAnimating;
    
    Orientation mInterfaceOrientation;
    float       mInterfaceAngle;       // animated, not always right-angle   
    ci::Vec2f   mTargetInterfaceSize;  // depends on mInterfaceOrientation
    float       mTargetInterfaceAngle; // normalized for shortest rotation animation
    
    // for lerping:
    float mLastOrientationChangeTime;
    float mLastUpdateTime;
    float mOrientationAnimationDuration;
    float mPrevInterfaceAngle;
    ci::Vec2f mPrevInterfaceSize;
};
