//
//  BloomScene.cpp
//  Kepler
//
//  Created by Tom Carden on 7/17/11.
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#import <UIKit/UIDevice.h>
#include "cinder/gl/gl.h"
#include "OrientationNode.h"
#include "BloomScene.h"

using namespace ci;

OrientationNodeRef OrientationNode::create()
{
    return OrientationNodeRef( new OrientationNode() );
}

OrientationNode::OrientationNode(): 
    mInterfaceAngle(0.0f),
    mTargetInterfaceSize(0.0f,0.0f),
    mTargetInterfaceAngle(0.0f),
    mLastOrientationChangeTime(0.0f),
    mLastUpdateTime(0.0f),
    mOrientationAnimationDuration(0.3f),
    mPrevInterfaceAngle(0.0f),
    mPrevInterfaceSize(0.0f,0.0f),
    mEnableAnimation(true),
    mCurrentlyAnimating(false)
{
}

OrientationNode::~OrientationNode()
{
    // TODO: stopNotifications
}

bool OrientationNode::addedToScene()
{
    // TODO: should this be a weak reference to avoid circular refs in the notification block?
    setupNotifications( this );
    return true;
}

void OrientationNode::orientationChanged( const Orientation &orientation )
{
    setInterfaceOrientation( orientation, mEnableAnimation );    
}

void OrientationNode::setInterfaceOrientation( const Orientation &orientation, bool animate )
{
    mInterfaceOrientation = orientation;
    
//    std::cout << "setting orientation node orientation to: " << getOrientationDescription(mInterfaceOrientation) << std::endl;
    
    const float TWO_PI = 2.0f * M_PI;
    
    // normalize current interface angle
    if (mInterfaceAngle < 0.0){
        float turns = floor( fabs( mInterfaceAngle / TWO_PI ) );
		mInterfaceAngle += turns * TWO_PI;
	}
    else if (mInterfaceAngle > TWO_PI){
        float turns = floor( mInterfaceAngle / TWO_PI );
		mInterfaceAngle -= turns * TWO_PI;
	}
    
    // get the facts
    Vec2f deviceSize = getRoot()->getWindowSize();
    float orientationAngle = getOrientationAngle( mInterfaceOrientation );
    
    // assign new targets
    mTargetInterfaceSize.x = fabs(deviceSize.x * cos(orientationAngle) + deviceSize.y * sin(orientationAngle));
    mTargetInterfaceSize.y = fabs(deviceSize.y * cos(orientationAngle) + deviceSize.x * sin(orientationAngle));
    mTargetInterfaceAngle = TWO_PI - orientationAngle;
    
    // make sure we're turning the right way
    if (abs(mTargetInterfaceAngle-mInterfaceAngle) > M_PI) {
        if (mTargetInterfaceAngle < mInterfaceAngle) {
            mTargetInterfaceAngle += TWO_PI;
        }
        else {
            mTargetInterfaceAngle -= TWO_PI;
        }
    }
    
    if (animate && mLastOrientationChangeTime >= 0.0f) {
        // remember current values for lerping later
        mPrevInterfaceAngle = mInterfaceAngle;
        mPrevInterfaceSize = getRoot()->getInterfaceSize();
        // and reset the counter
        mLastOrientationChangeTime = mLastUpdateTime;
    }
    else {
        // just jump to the animation's end in next update
        mLastOrientationChangeTime = -1;                
    }

    // ensure update() does the right thing
    mCurrentlyAnimating = true;
}

void OrientationNode::update( float elapsedSeconds )
{
    // animate transition
    if (mCurrentlyAnimating) {
        
        float t = elapsedSeconds - mLastOrientationChangeTime;

        if (t < mOrientationAnimationDuration) {
            float p = t / mOrientationAnimationDuration;
            getRoot()->setInterfaceSize( lerp(mPrevInterfaceSize, mTargetInterfaceSize, p) );
            mInterfaceAngle = lerp(mPrevInterfaceAngle, mTargetInterfaceAngle, p);
        }
        else {
            // ensure snap to final values
            getRoot()->setInterfaceSize( mTargetInterfaceSize );
            mInterfaceAngle = mTargetInterfaceAngle;
            mLastOrientationChangeTime = 0.0f; // used as sentinel in setInterfaceOrientation
            mCurrentlyAnimating = false;
        }
        
        // update matrix (for globalToLocal etc)
        mTransform.setToIdentity();
        mTransform.translate( Vec3f( getRoot()->getWindowSize() * 0.5f, 0 ) );
        mTransform.rotate( Vec3f( 0, 0, mInterfaceAngle ) );
        mTransform.translate( Vec3f( getRoot()->getInterfaceSize() * -0.5f, 0 ) );                        
    }    
    mLastUpdateTime = elapsedSeconds;
}

float OrientationNode::getOrientationAngle( const Orientation &orientation )
{
    switch (orientation) {
        case LANDSCAPE_LEFT:
            return M_PI * 1.5f;
        case UPSIDE_DOWN_PORTRAIT:
            return M_PI;
        case LANDSCAPE_RIGHT:
            return M_PI * 0.5f;
        case PORTRAIT:
        default:
            return 0.0;
    }
}

std::string OrientationNode::getOrientationDescription( const Orientation &orientation )
{
    switch (orientation) {
        case LANDSCAPE_LEFT:
            return "LANDSCAPE_LEFT";
        case UPSIDE_DOWN_PORTRAIT:
            return "UPSIDE_DOWN_PORTRAIT";
        case LANDSCAPE_RIGHT:
            return "LANDSCAPE_RIGHT";
        case PORTRAIT:
            return "PORTRAIT";
        default:
            return "UNKNOWN";
    }
}

OrientationNode::Orientation OrientationNode::convertOrientation( int uiDeviceOrientation )
{
    switch (uiDeviceOrientation) {
        case UIDeviceOrientationPortrait:            // Device oriented vertically, home button on the bottom
            return PORTRAIT;
        case UIDeviceOrientationPortraitUpsideDown:  // Device oriented vertically, home button on the top
            return UPSIDE_DOWN_PORTRAIT;
        case UIDeviceOrientationLandscapeLeft:       // Device oriented horizontally, home button on the right
            return LANDSCAPE_LEFT;
        case UIDeviceOrientationLandscapeRight:      // Device oriented horizontally, home button on the left
            return LANDSCAPE_RIGHT;
        case UIDeviceOrientationFaceUp:              // Device oriented flat, face up
            return FACE_UP;
        case UIDeviceOrientationFaceDown:             // Device oriented flat, face down        
            return FACE_DOWN;
        case UIDeviceOrientationUnknown:
        default:
            return UNKNOWN;
    }
}

// not a member function, so that blocks work correctly
void setupNotifications( OrientationNode *node )
{
    [[NSNotificationCenter defaultCenter] addObserverForName:@"UIDeviceOrientationDidChangeNotification"
                                                      object:nil 
                                                       queue:nil 
                                                  usingBlock: ^(NSNotification *notification)
    {
        UIDeviceOrientation orientation = [[UIDevice currentDevice] orientation];
        if (UIDeviceOrientationIsValidInterfaceOrientation(orientation)) {
            node->orientationChanged(OrientationNode::convertOrientation(orientation));
            // let's always make sure the task bar is shown on the correct side of the device            
            [UIApplication sharedApplication].statusBarOrientation = UIInterfaceOrientation(orientation);
        }
    }];
    // use initial taskbar orientation to derive a valid interface orientation
    node->setInterfaceOrientation( OrientationNode::convertOrientation([UIApplication sharedApplication].statusBarOrientation), false );
    [[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];            
}
