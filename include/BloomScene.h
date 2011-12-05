//
//  BloomScene.h
//  Kepler
//
//  Created by Tom Carden on 7/17/11.
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#pragma once
#include <map>
#include "cinder/Function.h"
#include "BloomNode.h"

// forward declare to avoid full include
//namespace cinder { namespace app {
//    class AppCocoaTouch;
//} }

class BloomScene : public BloomNode {
    
public:
    
    // use static create function and private constructor
    // for reasons why, see:
    // http://www.boost.org/doc/libs/1_47_0/libs/smart_ptr/sp_techniques.html#in_constructor 
    // http://www.boost.org/doc/libs/1_47_0/libs/smart_ptr/weak_ptr.htm#FAQ
    static BloomSceneRef create( ci::Vec2f interfaceSize, ci::Vec2f windowSize );
    
    virtual ~BloomScene();

    ci::Vec2f getWindowSize() { return mWindowSize; }
    void setWindowSize( ci::Vec2f windowSize ) { mWindowSize = windowSize; }    
    
    // these are used when handling orientation, by default they will just be the app window size
    ci::Vec2f getInterfaceSize() { return mInterfaceSize; }
    void setInterfaceSize( ci::Vec2f interfaceSize ) { mInterfaceSize = interfaceSize; }
    
//protected:

    bool touchesBegan( ci::app::TouchEvent event );
    bool touchesMoved( ci::app::TouchEvent event );
    bool touchesEnded( ci::app::TouchEvent event );
    
//    ci::app::AppCocoaTouch *mApp;
    
    ci::Vec2f mInterfaceSize, mWindowSize;
    
//    ci::CallbackId cbTouchesBegan, cbTouchesMoved, cbTouchesEnded;
    
private:
  
    BloomScene( ci::Vec2f interfaceSize, ci::Vec2f windowSize );
    
};